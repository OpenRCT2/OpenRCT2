FROM nfnty/arch-mini
MAINTAINER s0r00t
ENV DISPLAY :0

RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm git yajl cmake wget unzip wine
RUN pacman -S --noconfirm --needed base-devel

RUN useradd -mg root travis
RUN usermod -aG wheel travis
RUN sed -i 's/# %wheel ALL=(ALL) NOPASSWD: ALL/%wheel ALL=(ALL) NOPASSWD: ALL/g' /etc/sudoers

USER travis
WORKDIR /tmp
RUN curl -sL https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=package-query -o PKGBUILD
RUN makepkg
USER root
RUN pacman --noconfirm -U *.pkg.tar.xz
USER travis
RUN curl -sL https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=yaourt -o PKGBUILD
RUN makepkg
USER root
RUN pacman --noconfirm -U *.pkg.tar.xz

USER travis
RUN gpg --keyserver hkp://pool.sks-keyservers.net --recv-keys D9C4D26D0E604491 BB5869F064EA74AB 9D5EAAF69013B842 D605848ED7E69871 4DE8FF2A63C7CC90
RUN yaourt -S --noconfirm mingw-w64-gcc mingw-w64-zlib mingw-w64-pkg-config mingw-w64-openssl mingw-w64-jansson mingw-w64-libtasn1 mingw-w64-gmp mingw-w64-nettle mingw-w64-libffi mingw-w64-p11-kit mingw-w64-readline mingw-w64-gnutls mingw-w64-libunistring mingw-w64-termcap mingw-w64-libidn mingw-w64-curl mingw-w64-expat mingw-w64-libdbus mingw-w64-sdl2 mingw-w64-sdl2_ttf  mingw-w64-configure
USER root
RUN pacman --noconfirm -Rns $(pacman -Qttdq)
RUN pacman -Scc --noconfirm
WORKDIR /tmp
RUN rm -r *
WORKDIR /home/travis
USER travis
RUN git clone https://github.com/OpenRCT2/OpenRCT2
WORKDIR OpenRCT2
RUN ln -s /usr/i686-w64-mingw32/bin/libfreetype-6.dll
RUN ln -s /usr/i686-w64-mingw32/bin/libwinpthread-1.dll
RUN ln -s /usr/i686-w64-mingw32/bin/SDL2_ttf.dll
RUN ln -s /usr/i686-w64-mingw32/bin/SDL2.dll
RUN ln -s /usr/i686-w64-mingw32/bin/libbz2-1.dll
RUN ./install.sh
RUN ./build.sh
