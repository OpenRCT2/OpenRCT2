# Docker Development Environment

This repository can be built and run in a Linux container using the files in `docker/dev/`.

## What OpenRCT2 needs to build

From this repository's CMake files and Debian packaging metadata, Linux builds require:

- Build tools: C++20 compiler, `cmake` (3.24+), `ninja`, `pkg-config`, `git`
- Core libraries: `libzip`, `libpng`, `zlib`, `libzstd`, `libicu`, `nlohmann-json`
- Networking/security: `openssl`, `libcurl`
- Fonts/UI: `freetype`, `fontconfig`, `sdl2`
- Audio: `libflac`, `libogg`, `libvorbis`
- GUI rendering: OpenGL development headers (`libgl1-mesa-dev`)

The `docker/dev/Dockerfile` installs these dependencies.

## Build the container

```bash
cd docker/dev
docker compose build
```

To run as your local uid/gid (avoids root-owned files), pass them at build time:

```bash
USER_UID=$(id -u) USER_GID=$(id -g) docker compose build
```

## Start a development shell

```bash
cd docker/dev
docker compose run --rm openrct2-dev
```

## Build OpenRCT2 inside the container

Use the project build wrapper and disable install-time asset downloads for local development:

```bash
. scripts/setenv -q
scripts/build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDOWNLOAD_TITLE_SEQUENCES=OFF \
  -DDOWNLOAD_OBJECTS=OFF \
  -DDOWNLOAD_OPENSFX=OFF \
  -DDOWNLOAD_OPENMUSIC=OFF \
  -DDOWNLOAD_REPLAYS=OFF
```

## Run OpenRCT2 inside the container

Version check:

```bash
./bin/openrct2 --version
./bin/openrct2-cli --version
```

Run the game (requires original RCT2 assets):

```bash
./bin/openrct2 --rct2-data-path /opt/rct2-data
```

`/opt/rct2-data` is mounted read-only from `${RCT2_DATA_PATH}` on the host.

## Linux GUI forwarding (optional)

For GUI rendering from container to host X11:

```bash
xhost +local:docker
cd docker/dev
docker compose run --rm \
  -e DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v /dev/dri:/dev/dri \
  openrct2-dev \
  ./bin/openrct2 --rct2-data-path /opt/rct2-data
xhost -local:docker
```

If you only need server/headless workflows, use `openrct2-cli` and skip GUI forwarding.
