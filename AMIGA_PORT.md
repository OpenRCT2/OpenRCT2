# OpenRCT2 on AmigaOS 3.x (68k)

This branch (`amiga`) ports OpenRCT2 to big-endian 68k AmigaOS 3.x. It is a
fork of https://github.com/OpenRCT2/OpenRCT2 and inherits its GPLv3 licence.
Build instructions: `distribution/amiga/BUILDING.md`. Tester notes:
`distribution/amiga/TESTER-GUIDE.md`.

## Status

- **Simulation is bit-exact with the x86-64/arm64 build**: identical entity
  checksums through 5000 ticks on multiple parks, verified on a 68040
  against a native build of the same tree.
- **Native loading of `.park`, RCT2 `.sc6`/`.sv6` and RCT1 `.sc4`**, all
  bit-exact at tick 0 (RCT1 base, Added Attractions and Loopy Landscapes).
- **Graphical UI** on a Picasso96/CyberGraphX 8-bit RTG screen with Intuition
  mouse and keyboard input, through a small SDL2 API shim
  (`src/openrct2-ui/platform/amiga/`), so the upstream UI code is untouched.
  ~25 fps at 640x480 on an emulated 68040; faster on PiStorm and Vampire.
- **Sound and music** through `ahi.device` (AHI), using upstream's mixer with
  integer fixed-point effect loops for the soft-float 68k. Ogg/Vorbis and
  FLAC are not built, so OpenRCT2's own music packs do not play; the RCT2
  `css*.dat` files do.

Not done: mouse cursor shapes and clipboard (Intuition pointer only), the
optional Ogg music packs, networking, scripting, hardware FPU builds.
Memory: about 256 MB of Fast RAM in use.

## How the port is structured

- **Byte order.** The RCT file formats are little-endian. Scalars read through
  `IStream` are swapped (`core/IStream.hpp`, `core/Endianness.h`); on-disk
  structs convert themselves (`LittleEndianToHost()`); the raw RCT2/RCT1 blobs
  are swapped in place after decoding (`rct2/S6Importer.cpp`,
  `rct1/S4Importer.cpp`, `park/ParkFile.cpp` tile elements). Unions that alias
  a wide integer over byte members are ordered by `__BYTE_ORDER__`
  (`ride/Ride.h`, `entity/Peep.h`, `entity/Guest.h`, `ride/Vehicle.h`, ...).
  `core/ChecksumStream` hashes little-endian words so checksums match across
  hosts. Portable fixes found on the way (Formatter argument storage, SC4
  decode word rotation, title-script byte read, `ChecksumStream` fast paths)
  apply to any big-endian host.
- **Platform.** `platform/Platform.Amiga.cpp`, `platform/amiga_os.c`
  (timer, sleep, trace), `platform/amiga_compat.cpp` (libnix/libstdc++
  gaps). No threads: `core/JobPool.cpp` runs jobs inline.
- **UI backend.** `src/openrct2-ui/platform/amiga/`: `amiga_ui.c` (screen,
  chunky blits, palette, IDCMP input), `amiga_audio.c` (AHI), `sdl_shim.cpp`
  and `sdl_shim_audio.cpp` (the SDL2 subset the UI calls), `sdl2/` (the
  matching headers), `AmigaDrawingEngine.cpp` (X8 rasteriser presented with
  dirty-rectangle blits), `UiContext.Amiga.cpp`.

## Contributing back

The endian fixes in `core/`, `localisation/`, `sawyer_coding/`, `rct1/`,
`rct2/` and the union ordering are candidates for upstream on their own;
the AmigaOS platform and UI backend are self-contained under `platform/`
and `platform/amiga/` and guarded by `AMIGA`/`__amigaos__`.
