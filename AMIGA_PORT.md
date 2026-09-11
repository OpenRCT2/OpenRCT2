# OpenRCT2 on 68k AmigaOS 3.2

A port of OpenRCT2 (RollerCoaster Tycoon 2 engine, C++20) to big-endian
68k AmigaOS 3.2, built with bebbo's amiga-gcc **GCC 16.2.0b**. This branch
(`amiga`) is a fork of OpenRCT2; upstream is https://github.com/OpenRCT2/OpenRCT2
(GPLv3, which this inherits).

## What works
- **Headless simulation is bit-exact with the x86-64/arm64 build** — identical
  entity checksums through 5000 ticks on multiple parks. The whole little-endian
  file format is handled by an endian layer (scalar stream swaps, self-converting
  structs, per-type tile-element and entity byte-order fixes, host-order checksum).
- **Full graphical UI** on a Picasso96/CyberGraphX 8-bit RTG screen with
  Intuition mouse/keyboard input, via an SDL2 shim (`src/openrct2-ui/platform/amiga/`).
  The title sequence, parks and menus render and respond; ~27 fps at 640x480 on a
  68040 (Amiga emulator).
- **Native `.park` loading** (bit-exact) and **native SC6/SV6 scenario loading**
  (big-endian byte-swap of the raw RCT2 struct; scenario index uses a fast
  Info-only scan).

## Not done
- SC4 (RCT1) loading; audio; mouse cursor shapes; SC6 simulation is
  non-deterministic even on x86 (an uninitialised import field), so cross-host
  bit-exact *simulation* verification uses the `.park` path.

## Building
Cross-compile with GCC 16.2 for m68k-amigaos. See the sibling project
`amiopenrtc2` (build scripts under `spike/`, toolchain recipe in AllAmigaTooling)
for the exact CMake toolchain file, flags and deploy tooling.

## Key commits (this branch)
Endian layer, GCC 16 build fixes, union byte-order fixes, ChecksumStream
little-endian definition, P96/Intuition UI backend, title-parser endian fix,
maze tile swap, native SC6 loader. See `git log`.
