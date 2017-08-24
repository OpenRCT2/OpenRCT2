FROM nfnty/arch-mini

RUN echo -e "[archlinuxfr]\nSigLevel = Never\nServer = http://repo.archlinux.fr/\$arch" >> /etc/pacman.conf
RUN sed -i '/#\[multilib\]/,/#Include = \/etc\/pacman.d\/mirrorlist/ s/#//' /etc/pacman.conf
RUN pacman -Syyu --noconfirm
RUN pacman -S --noconfirm git curl jshon expac yajl wget unzip cmake && pacman -Sc
RUN yes | pacman -S gcc-multilib gcc-libs-multilib && pacman -Sc
RUN pacman -S --noconfirm --needed make pkg-config patch yaourt grep file sudo gawk fakeroot gzip m4 which util-linux && pacman -Sc

RUN useradd -mg root travis
RUN usermod -aG wheel travis
RUN sed -i 's/# %wheel ALL=(ALL) NOPASSWD: ALL/%wheel ALL=(ALL) NOPASSWD: ALL/g' /etc/sudoers

RUN pacman -S --noconfirm lib32-curl lib32-sdl2 lib32-speex lib32-fontconfig lib32-openssl lib32-libpng && pacman -Sc
USER travis
RUN yaourt -S --noconfirm lib32-jansson lib32-libzip1 && rm -rf /tmp/yaourt-tmp-travis && sudo pacman -Sc
