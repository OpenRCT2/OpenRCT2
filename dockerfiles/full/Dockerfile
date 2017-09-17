FROM nfnty/arch-mini

RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm git curl jshon expac ninja
RUN pacman -S --noconfirm --needed base-devel
RUN pacman -S --noconfirm yajl

RUN useradd -mg root travis
RUN usermod -aG wheel travis
RUN sed -i 's/# %wheel ALL=(ALL) NOPASSWD: ALL/%wheel ALL=(ALL) NOPASSWD: ALL/g' /etc/sudoers

WORKDIR /tmp
USER travis
RUN curl -sLO https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=package-query
RUN mv PKGBUILD?h=package-query PKGBUILD
RUN makepkg
USER root
RUN pacman --noconfirm -U *.pkg.tar.xz
USER travis
RUN curl -sLO https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=yaourt
RUN mv PKGBUILD?h=yaourt PKGBUILD
RUN makepkg
USER root
RUN pacman --noconfirm -U *.pkg.tar.xz

RUN pacman -R --noconfirm gcc
RUN yes | pacman -S gcc-libs-multilib
RUN pacman -S --noconfirm gcc-multilib cmake
RUN pacman -S --noconfirm cmake
USER travis
RUN yaourt -S --noconfirm mingw-w64-headers
RUN yaourt -S --noconfirm mingw-w64-gcc
RUN yaourt -S --noconfirm wine
RUN yaourt -S --noconfirm mingw-w64-zlib mingw-w64-pkg-config
RUN gpg --keyserver hkp://pool.sks-keyservers.net --recv-keys D9C4D26D0E604491 BB5869F064EA74AB 9D5EAAF69013B842 D605848ED7E69871 4DE8FF2A63C7CC90
RUN yaourt -S --noconfirm mingw-w64-openssl
RUN yaourt -S --noconfirm mingw-w64-jansson
RUN yaourt -S --noconfirm mingw-w64-libtasn1
RUN yaourt -S --noconfirm mingw-w64-gmp
RUN yaourt -S --noconfirm mingw-w64-nettle
RUN yaourt -S --noconfirm mingw-w64-libffi
RUN yaourt -S --noconfirm mingw-w64-p11-kit
RUN yaourt -S --noconfirm mingw-w64-readline
RUN yaourt -S --noconfirm mingw-w64-gnutls
RUN yaourt -S --noconfirm mingw-w64-libunistring
RUN yaourt -S --noconfirm mingw-w64-termcap
RUN yaourt -S --noconfirm mingw-w64-gettext
RUN yaourt -S --noconfirm mingw-w64-libidn
RUN yaourt -S --noconfirm mingw-w64-curl
RUN yaourt -S --noconfirm mingw-w64-expat
RUN yaourt -S --noconfirm mingw-w64-libdbus
RUN yaourt -S --noconfirm mingw-w64-sdl2
RUN yaourt -S --noconfirm mingw-w64-sdl2_ttf
RUN yaourt -S --noconfirm wget unzip
RUN yaourt -S --noconfirm lib32-jansson lib32-curl lib32-sdl2 lib32-sdl2_ttf
RUN yaourt -S --noconfirm lib32-speex
#RUN git clone https://github.com/OpenRCT2/OpenRCT2
#WORKDIR /tmp/OpenRCT2
#RUN ./install.sh
#RUN ./build.sh
