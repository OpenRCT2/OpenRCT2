FROM nfnty/arch-mini

RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm base-devel gcc gcc-libs git curl jshon expac yajl wget unzip cmake ninja && pacman -Sc
RUN pacman -S --noconfirm systemd curl ccache sdl2 speex fontconfig openssl libpng jansson libzip icu && pacman -Sc
RUN pacman -S --noconfirm --needed make pkg-config
