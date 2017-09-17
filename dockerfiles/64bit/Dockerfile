FROM nfnty/arch-mini

RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm gcc gcc-libs git curl jshon expac yajl wget unzip cmake ninja && pacman -Sc
RUN pacman -S --noconfirm curl sdl2 speex fontconfig openssl libpng jansson libzip && pacman -Sc
RUN pacman -S --noconfirm --needed make pkg-config
