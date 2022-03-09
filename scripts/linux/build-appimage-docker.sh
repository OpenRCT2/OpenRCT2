#!/bin/bash -ex

REPO_ROOT=$(readlink -f $(dirname "$0")/../..)

docker run --rm -it -e NO_CLEANUP=1 -e CI=1 --device /dev/fuse:mrw -v $(readlink -f .):/ws openrct2/openrct2:ubuntu_amd64 bash -exc "
cd /ws
apt-get update
apt-get install -y libcairo2
scripts/linux/build-appimage.sh
"
