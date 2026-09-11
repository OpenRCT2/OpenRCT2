#!/bin/sh
# Assemble the AmigaOS tester archive:  dist-testers/OpenRCT2-<ver>-amiga68k.lha
#   OpenRCT2.info            drawer icon
#   OpenRCT2/OpenRCT2        launcher script (IconX icon)   -- RCT2 data via  Assign RCT2:
#   OpenRCT2/bin/openrct2    the 68k executable (never next to the launcher: AmigaDOS is case-insensitive)
#   OpenRCT2/data            OpenRCT2's own data (objects, languages, title sequences, g2, ...)
#   OpenRCT2/user            config.ini with Amiga defaults; caches land here on first run
# The original RCT2 data is NOT included (not redistributable).
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
UP="$(cd "$HERE/../.." && pwd)"          # upstream checkout (git)
PROJ="$(cd "$UP/.." && pwd)"             # amiopenrtc2 project root
BIN="${BIN:-$PROJ/build-68k/openrct2}"
DATA="${DATA:-$PROJ/dist/OpenRCT2/data}"
TAG="${TAG:-test1}"
SHA="$(git -C "$UP" log -1 --format=%h)"
NAME="OpenRCT2-0.5.5-$TAG-$SHA-amiga68k"
OUT="$PROJ/dist-testers"
STAGE="$OUT/stage"
LHACLI="$HOME/Development/AmigaDiskKit/.build/arm64-apple-macosx/release/AmigaDiskCLI"
[ -f "$BIN" ] || { echo "no binary at $BIN"; exit 1; }
[ -d "$DATA" ] || { echo "no data at $DATA (run spike/stage-dist.sh first)"; exit 1; }
rm -rf "$STAGE"; mkdir -p "$STAGE/OpenRCT2/bin" "$STAGE/OpenRCT2/user"
cp "$BIN" "$STAGE/OpenRCT2/bin/openrct2"
cp -R "$DATA" "$STAGE/OpenRCT2/data"
cp "$HERE/OpenRCT2" "$STAGE/OpenRCT2/OpenRCT2"
cp "$HERE/OpenRCT2-launcher.info" "$STAGE/OpenRCT2/OpenRCT2.info"
cp "$HERE/OpenRCT2-drawer.info" "$STAGE/OpenRCT2.info"
cp "$HERE/README.txt" "$HERE/README.txt.info" "$STAGE/OpenRCT2/"
cp "$HERE/user/config.ini" "$STAGE/OpenRCT2/user/config.ini"
printf 'OpenRCT2 0.5.5 AmigaOS/68k tester build %s (git %s), built %s\n' "$TAG" "$SHA" "$(date -u +%Y-%m-%dT%H:%MZ)" > "$STAGE/OpenRCT2/VERSION"
find "$STAGE" -name .DS_Store -delete
rm -f "$OUT/$NAME.lha"
"$LHACLI" lha create "$OUT/$NAME.lha" "$STAGE"
ls -la "$OUT/$NAME.lha"; du -sh "$STAGE"
echo "$OUT/$NAME.lha"
