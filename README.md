# ifdtool-thinkpad — HAP Bit Fork for 8th–14th Gen Intel Platforms

Fork of [coreboot's ifdtool](https://github.com/coreboot/coreboot/tree/main/util/ifdtool) with hardware- and datasheet-confirmed fixes for reading and setting the Intel ME HAP (High Assurance Platform) disable bit across Cannon Lake through Meteor Lake platforms.

---

## The Problem

Upstream ifdtool has a silent bug affecting 8th–10th gen Intel platforms: both the display path (`-d`) and the write path (`-M 1`) unconditionally use `PCHSTRP0` bit 16 as the HAP bit location for all IFD v2 platforms.

This is wrong for Cannon Lake (CNL), Ice Lake (ICL), and Comet Lake (CML). On these platforms the HAP bit lives in **`PCHSTRP28` bit 16**, not `PCHSTRP0`. The result is:

- `ifdtool -d` reports `HAP bit is not set` even on a correctly patched image
- `ifdtool -M 1` silently writes to the wrong strap and does nothing — the binary is modified but ME is not disabled

This bug has likely caused years of failed ME disable attempts on ThinkPads and similar 8th–10th gen laptops where users believed ifdtool had worked.

---

## The Fix — HAP Bit Locations by Platform

All confirmed by binary diffing firmware images before and after HAP was set by Intel's Flash Image Tool (FIT), or by direct Intel PCH datasheet analysis:

| Platform flag | Generation | ME Version | HAP Location | Source |
|---------------|-----------|------------|--------------|--------|
| `-p cnl` | 8th/9th gen (Cannon Lake LP) | ME 12 | `PCHSTRP28` bit 16 | ✅ Hardware confirmed |
| `-p icl` | 10th gen (Ice Lake LP) | ME 13 | `PCHSTRP28` bit 16 | ✅ Datasheet confirmed (Doc 615170) |
| `-p cml` | 10th gen (Comet Lake LP) | ME 14 | `PCHSTRP28` bit 16 | ✅ Hardware confirmed |
| `-p tgl` | 11th gen (Tiger Lake) | ME 15 | `PCHSTRP31` bit 16 | ✅ Datasheet confirmed (Doc 635218) |
| `-p adl` | 12th gen (Alder Lake) | ME 16 | `PCHSTRP31` bit 16 | ✅ Datasheet confirmed (Doc 648364) |
| `-p rpl` | 13th gen (Raptor Lake) | ME 16.1 | `PCHSTRP31` bit 16 | ✅ Datasheet confirmed (Doc 743835) |
| `-p mtl` | 14th gen (Meteor Lake) | ME 18 | `PCHSTRP31` bit 16 | ⚠️ Unconfirmed — descriptor layout changed, placeholder only |

Hardware confirmation method for CML-LP (ThinkPad X13 Gen1, ME 14.1.77.2497):

```
fpsba = 0x100
PCHSTRP28 is at fpsba + 0x70 (= 0x170, index 28 * 4 bytes)
stock   PCHSTRP28 = 0x801801b8
patched PCHSTRP28 = 0x801901b8
diff    = 0x00010000 = bit 16
```

Exactly one byte changed in the entire image. Upstream ifdtool would have written to `fpsba + 0x00` (PCHSTRP0) — a completely different register.

ADL/RPL confirmation (Doc 648364, 743835):
```
fpsba = 0x100
PCHSTRP31 at fpsba + 0x7C = 0x17C
Descriptor byte 0x017E = byte 2 of PCHSTRP31 = bit 16
```

---

## Changes vs Upstream

### `ifdtool.h`

- Added `PLATFORM_CML`, `PLATFORM_TGL`, `PLATFORM_ADL`, `PLATFORM_MTL` to `enum platform`
- `PLATFORM_ADL` covers both ADL (12th gen) and RPL (13th gen) — `-p rpl` is an alias
- `PLATFORM_MTL` is a placeholder — HAP location unconfirmed for Meteor Lake

### `ifdtool.c`

- `ifd2_platform_to_chipset()` — CML, TGL, ADL, RPL, MTL all mapped to correct chipset series
- `is_platform_ifd_2()` — all new platforms added to IFD v2 list
- `dump_fpsba()` — HAP read/display path selects correct strap per platform, with named output
- `fpsba_set_altmedisable()` — HAP write path (`-M 1`) writes to correct strap per platform
- `-p` argument parser — `cml`, `tgl`, `adl`, `rpl`, `mtl` added as valid platform strings
- `print_usage()` — all new platforms documented in help output with HAP location and confirmation status

---

## Building

No coreboot tree required. All dependencies are vendored in this repo.

**Prerequisites** (one-time):
```bash
# Debian/Ubuntu
sudo apt install build-essential

# Fedora/RHEL
sudo dnf install gcc make
```

**Build:**
```bash
git clone https://github.com/MangoKiwiPlumGrape/ifdtool_thinkpad
cd ifdtool_thinkpad
make
```

---

## Usage

### Check HAP status

```bash
# 8th/9th gen Cannon Lake
./ifdtool -d -p cnl dump.bin

# 10th gen Ice Lake
./ifdtool -d -p icl dump.bin

# 10th gen Comet Lake LP (ThinkPad X13/T14/L14 Gen1)
./ifdtool -d -p cml dump.bin

# 11th gen Tiger Lake
./ifdtool -d -p tgl dump.bin

# 12th gen Alder Lake
./ifdtool -d -p adl dump.bin

# 13th gen Raptor Lake
./ifdtool -d -p rpl dump.bin
```

Output correctly reports which strap was checked:
```
HAP bit is set (checked PCHSTRP28 bit 16)
```
or
```
HAP bit is set (checked PCHSTRP31 bit 16)
```

Upstream ifdtool on the same image would incorrectly report:
```
HAP bit is not set
```

### Set HAP bit (disable ME)

```bash
./ifdtool -M 1 -p cml dump.bin -O hap_patched.bin   # Comet Lake
./ifdtool -M 1 -p adl dump.bin -O hap_patched.bin   # Alder Lake
./ifdtool -M 1 -p rpl dump.bin -O hap_patched.bin   # Raptor Lake
```

### Unset HAP bit (re-enable ME)

```bash
./ifdtool -M 0 -p cml dump.bin -O me_enabled.bin
```

### All other ifdtool operations

All other flags (`-d`, `-u`, `-l`, `-x`, `-i`, etc.) are unchanged from upstream. Run `./ifdtool -h` for the full option list.

---

## Verified Working Devices

| Device | Platform | ME Version | Result |
|--------|----------|------------|--------|
| ThinkPad X1 Carbon 6th–9th gen | CNL (8th/9th gen LP) | ME 12.0.x | ✅ Hardware flashed, ME gone |
| ThinkPad X13 Gen1 | CML (10th gen LP) | ME 14.1.77.2497 | ✅ Hardware flashed, ME gone |

After a successful flash with HAP set:
- BIOS ME version field: blank
- `lspci | grep -i mei` → no output
- intelmetool → no interface found
- MEAnalyzer (on file) → `HAP/AltMeDisable: Yes`

---

## MTL Warning

Meteor Lake (`-p mtl`) is included as a **placeholder only**. The MTL descriptor layout is fundamentally different from all previous platforms — there are no PCH Straps at `0x100`. The HAP bit location on MTL has not been empirically confirmed. Do not use `-p mtl -M 1` on a real MTL image without first verifying the HAP offset from a known-good MTL firmware dump.

---

## Using with me_cleaner-thinkpad

This fork pairs with [me_cleaner-thinkpad](https://github.com/MangoKiwiPlumGrape/me_cleaner_thinkpad), which has the same HAP offset fixes for the same platform range. The two tools are independently useful:

- **ifdtool-thinkpad** — verify HAP status and set it via internal flash (software, no chip clip needed)
- **me_cleaner-thinkpad** — set HAP via external programmer (ch341a + flashrom)

Both have been corrected to use the right strap on CNL/ICL/CML (PCHSTRP28) and TGL/ADL/RPL (PCHSTRP31).

---

## Why PCHSTRP28 on CNL/ICL/CML?

Intel's Flash Image Tool (FIT) sets HAP by writing to `PCHSTRP28` bit 16 on these platforms. Confirmed by binary-diffing a stock image against one processed by FIT with HAP enabled — exactly one 4-byte region changed, at offset `fpsba + 0x70` (PCHSTRP28, since each strap is 4 bytes: `28 × 4 = 0x70`).

The upstream ifdtool code predates widespread Cannon Lake / Comet Lake adoption and was never updated to reflect the strap relocation that occurred with the 300-series PCH.

---

## License

GPL-2.0-only — same as upstream ifdtool.

Copyright (C) 2011 Google Inc. (original)  
Modifications copyright (C) 2026 MangoKiwiPlumGrape
