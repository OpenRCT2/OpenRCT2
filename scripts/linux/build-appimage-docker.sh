#! /bin/bash

set -ex

REPO_ROOT=$(readlink -f $(dirname "$0")/../..)

docker run --rm -it -e NO_CLEANUP=1 -e CI=1 --device /dev/fuse:mrw -v $(readlink -f .):/ws openrct2/openrct2:ubuntu_amd64 bash -xc "
cd /ws
apt-get update
apt-get install -y wget libcairo2
bash -xe scripts/linux/build-appimage.sh
"
