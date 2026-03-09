# ifdtool — HAP Bit Fix for 8th–10th Gen Intel Platforms

Fork of [coreboot's ifdtool](https://github.com/coreboot/coreboot/tree/main/util/ifdtool) with hardware-confirmed fixes for reading and setting the Intel ME HAP (High Assurance Platform) disable bit on Cannon Lake, Ice Lake, and Comet Lake platforms.

---

## The Problem

Upstream ifdtool has a silent bug affecting 8th–10th gen Intel platforms: both the display path (`-d`) and the write path (`-M 1`) unconditionally use `PCHSTRP0` bit 16 as the HAP bit location for all IFD v2 platforms.

This is wrong for Cannon Lake (CNL), Ice Lake (ICL), and Comet Lake (CML). On these platforms the HAP bit lives in **`PCHSTRP28` bit 16**, not `PCHSTRP0`. The result is:

- `ifdtool -d` reports `HAP bit is not set` even on a correctly patched image
- `ifdtool -M 1` silently writes to the wrong strap and does nothing — the binary is modified but ME is not disabled

This bug has likely caused years of failed ME disable attempts on ThinkPads and similar 8th–10th gen laptops where users believed ifdtool had worked.

---

## The Fix

Hardware-confirmed HAP bit locations determined by binary diffing firmware images before and after HAP was set by Intel's own Flash Image Tool (FIT):

| Platform | Generation | ME Version | HAP Location | Status |
|----------|-----------|------------|--------------|--------|
| CNL (Cannon Lake LP) | 8th/9th gen | ME 12 | `PCHSTRP28` bit 16 | ✅ Hardware confirmed |
| ICL (Ice Lake) | 10th gen | ME 13 | `PCHSTRP28` bit 16 | ⚠️ Mapped by analogy with CML (same PCH series) |
| CML (Comet Lake LP) | 10th gen | ME 14 | `PCHSTRP28` bit 16 | ✅ Hardware confirmed |
| All other IFD v2 | Various | Various | `PCHSTRP0` bit 16 | Upstream default (unchanged) |

Hardware confirmation method for CML-LP (ThinkPad X13 Gen1, ME 14.1.77.2497):

```
fpsba = 0x100
PCHSTRP28 is at fpsba + 0x70 (= 0x170, index 28 * 4 bytes)
stock  PCHSTRP28 = 0x801801b8
patched PCHSTRP28 = 0x801901b8
diff = 0x00010000 = bit 16
```

Exactly one byte changed in the entire image. `ifdtool -M 1` upstream would have written to `fpsba + 0x00` (PCHSTRP0) — a completely different register.

---

## Changes vs Upstream

### `ifdtool.h`

- Added `PLATFORM_CML` to `enum platform`

### `ifdtool.c`

- `ifd2_platform_to_chipset()` — CML now maps to `CHIPSET_400_SERIES_ICE_POINT`
- `is_platform_ifd_2()` — CML added to the IFD v2 platform list
- `dump_fpsba()` — HAP read/display path now selects `PCHSTRP28` for CNL/ICL/CML, with output that names the strap checked
- `fpsba_set_altmedisable()` — HAP write path (`-M 1`) now writes to `PCHSTRP28` for CNL/ICL/CML
- `-p` argument parser — added `cml` as a valid platform string
- `print_usage()` — `cml` documented in help output

---

## Building

ifdtool depends on two headers from the coreboot source tree. The quickest way to get them without cloning the full repo:

```bash
cd ~/your-ifdtool-directory

# grab headers
mkdir -p commonlib/bsd
curl -o commonlib/helpers.h \
  https://raw.githubusercontent.com/coreboot/coreboot/main/src/commonlib/include/commonlib/helpers.h
curl -o commonlib/bsd/helpers.h \
  https://raw.githubusercontent.com/coreboot/coreboot/main/src/commonlib/bsd/include/commonlib/bsd/helpers.h
curl -o commonlib/bsd/compiler.h \
  https://raw.githubusercontent.com/coreboot/coreboot/main/src/commonlib/bsd/include/commonlib/bsd/compiler.h
curl -o fmap.h \
  https://raw.githubusercontent.com/coreboot/coreboot/main/util/cbfstool/fmap.h

# build
make CFLAGS="-I."
```

Or via a sparse clone if you prefer everything in one step:

```bash
git clone --depth=1 --filter=blob:none --sparse \
  https://github.com/coreboot/coreboot.git cb-headers
cd cb-headers
git sparse-checkout set src/commonlib util/cbfstool
cp -r src/commonlib/include/commonlib ~/your-ifdtool-directory/commonlib
cp util/cbfstool/fmap.h ~/your-ifdtool-directory/
cd ~/your-ifdtool-directory
make CFLAGS="-I."
```

---

## Usage

### Check HAP status

```bash
# 8th/9th gen (Cannon Lake LP, ME 12)
./ifdtool -d -p cnl stock.bin

# 10th gen Ice Lake (ME 13)
./ifdtool -d -p icl stock.bin

# 10th gen Comet Lake LP (ME 14) — ThinkPad X13 Gen1, T14 Gen1, L14 Gen1 etc.
./ifdtool -d -p cml stock.bin
```

Output will now correctly report the strap that was checked:

```
HAP bit is set (checked PCHSTRP28 bit 16)
```

Upstream ifdtool on the same image would incorrectly report:

```
HAP bit is not set
```

### Set HAP bit (disable ME)

```bash
# 8th/9th gen
./ifdtool -M 1 -p cnl stock.bin -O hap_patched.bin

# 10th gen Ice Lake
./ifdtool -M 1 -p icl stock.bin -O hap_patched.bin

# 10th gen Comet Lake LP
./ifdtool -M 1 -p cml stock.bin -O hap_patched.bin
```

### Unset HAP bit (re-enable ME)

```bash
./ifdtool -M 0 -p cml stock.bin -O me_enabled.bin
```

### All other ifdtool operations

All other flags (`-d`, `-u`, `-l`, `-x`, `-i`, etc.) are unchanged from upstream. Refer to `./ifdtool -h` for the full option list.

---

## Verified Working Devices

| Device | Platform | ME Version | Result |
|--------|----------|------------|--------|
| ThinkPad X1 Carbon 6th–9th gen | CNL (8th/9th gen LP) | ME 12.0.x | ✅ Hardware flashed, ME gone |
| ThinkPad X13 Gen1 | CML (10th gen LP) | ME 14.1.77.2497 | ✅ Hardware flashed, ME gone |

After a successful flash with HAP set:
- BIOS ME version field: blank
- `lspci | grep -i mei` → no output
- MEInfo / intelmetool → no interface found
- MEAnalyzer (on file) → `HAP/AltMeDisable: Yes`

---

## Using with me_cleaner

This fork pairs well with [me_cleaner-thinkpad](https://github.com/your-username/me_cleaner-thinkpad), a fork of me_cleaner with the same HAP offset fixes for 8th–10th gen ThinkPads. The two tools are independently useful:

- **ifdtool** — for verifying HAP status and setting it via software flash (internal programmer)
- **me_cleaner** — for setting HAP via external programmer (ch341a, flashrom etc.)

Both have been corrected to use `PCHSTRP28` on CNL/ICL/CML platforms.

---

## Why PCHSTRP28?

The Intel Flash Image Tool (FIT) sets HAP by writing to `PCHSTRP28` bit 16 on these platforms. This was confirmed by binary-diffing a stock firmware image against one processed by FIT with HAP enabled — exactly one 4-byte region changed, at offset `fpsba + 0x70` (which is `PCHSTRP28`, since each strap is 4 bytes: `28 * 4 = 0x70`).

The upstream ifdtool code predates widespread Cannon Lake / Comet Lake adoption and was never updated to reflect the strap relocation that occurred starting with the 300-series PCH.

---

## License

GPL-2.0-only — same as upstream ifdtool.

Copyright (C) 2011 Google Inc. (original)  
Modifications copyright (C) 2026 MangoKiwiPlumGrape
