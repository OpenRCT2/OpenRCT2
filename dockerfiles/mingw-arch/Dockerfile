FROM nfnty/arch-mini:latest

# Based on https://github.com/haffmans/docker-mingw-qt5/blob/master/Dockerfile

# Update base system
RUN    pacman -Sy --noconfirm --noprogressbar archlinux-keyring \
    && pacman-key --populate \
    && pacman -Su --noconfirm --noprogressbar pacman \
    && pacman-db-upgrade \
    && pacman -Su --noconfirm --noprogressbar ca-certificates \
    && trust extract-compat \
    && pacman -Syyu --noconfirm --noprogressbar \
    && (echo -e "y\ny\n" | pacman -Scc)

# Install yaourt
RUN echo "[archlinuxfr]" >> /etc/pacman.conf && \
    echo "SigLevel = Never" >> /etc/pacman.conf && \
    echo "Server = http://repo.archlinux.fr/x86_64" >> /etc/pacman.conf &&\
    pacman -Sy

# Add some useful packages to the base system
RUN pacman -S --noconfirm --noprogressbar \
        make \
        git \
        sudo \
        wget \
        yaourt \
        awk \
        sudo \
        fakeroot \
        file \
        patch \
        ninja \
    && (echo -e "y\ny\n" | pacman -Scc)

# Add mingw-repo
RUN    echo "[mingw-w64]" >> /etc/pacman.conf \
    && echo "SigLevel = Optional TrustAll" >> /etc/pacman.conf \
    && echo "Server = http://downloads.sourceforge.net/project/mingw-w64-archlinux/\$arch" >> /etc/pacman.conf \
    && pacman -Sy

# Install MingW packages
RUN pacman -S --noconfirm --noprogressbar \
        mingw-w64-binutils \
        mingw-w64-crt \
        mingw-w64-gcc \
        mingw-w64-headers \
        mingw-w64-winpthreads \
        mingw-w64-cmake \
        mingw-w64-fontconfig \
        mingw-w64-freetype2 \
        mingw-w64-libiconv \
        mingw-w64-libpng \
        mingw-w64-openssl \
        mingw-w64-pkg-config \
        mingw-w64-sdl2 \
        mingw-w64-tools \
        mingw-w64-zlib \
        mingw-w64-sdl2_ttf \
        mingw-w64-curl \
    && (echo -e "y\ny\n" | pacman -Scc)

RUN useradd --create-home --comment "Arch Build User" build && \
    groupadd sudo && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers; \
    echo 'Defaults:nobody !requiretty' >> /etc/sudoers; \
    gpasswd -a build sudo
USER build
WORKDIR /tmp
# Overrides from arch-base
ENV HOME /home/build

RUN yaourt -S --noconfirm --noprogressbar \
        mingw-w64-jansson \
        aur/mingw-w64-libzip \
        aur/mingw-w64-speexdsp \
    && (echo -e "y\ny\n" | sudo pacman -Scc)
