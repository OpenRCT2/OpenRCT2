FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bash-completion \
        build-essential \
        ca-certificates \
        ccache \
        clang \
        clang-format \
        cmake \
        curl \
        gdb \
        git \
        lldb \
        ninja-build \
        nlohmann-json3-dev \
        pkg-config \
        python3 \
        sudo \
        unzip \
        zip \
        zlib1g-dev \
        libcurl4-openssl-dev \
        libflac-dev \
        libfontconfig1-dev \
        libfreetype6-dev \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        libicu-dev \
        libogg-dev \
        libpng-dev \
        libsdl2-dev \
        libssl-dev \
        libvorbis-dev \
        libzip-dev \
        libzstd-dev \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd --force --gid "${USER_GID}" "${USERNAME}" \
    && if id -u "${USERNAME}" >/dev/null 2>&1; then \
        usermod --uid "${USER_UID}" --gid "${USER_GID}" "${USERNAME}"; \
    else \
        useradd --uid "${USER_UID}" --gid "${USER_GID}" -m --non-unique "${USERNAME}"; \
    fi \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/${USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${USERNAME}"

ENV CC=clang
ENV CXX=clang++
ENV CCACHE_DIR=/home/${USERNAME}/.ccache
ENV PATH=/usr/lib/ccache:${PATH}

WORKDIR /workspaces/OpenRCT2
USER ${USERNAME}

CMD ["/bin/bash"]
