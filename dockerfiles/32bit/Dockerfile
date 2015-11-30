FROM nfnty/arch-mini

RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm git curl jshon expac yajl wget unzip cmake
RUN pacman -S --noconfirm --needed base-devel

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
RUN pacman -S --noconfirm gcc-multilib
USER travis
RUN yaourt -S --noconfirm lib32-jansson lib32-curl lib32-sdl2 lib32-sdl2_ttf lib32-speex
