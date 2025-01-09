FROM docker.io/library/fedora:41 AS builder

RUN dnf update -y && dnf install -y git cmake make gcc g++ nlohmann-json-devel autoreconf libtool openssl-devel libcurl-devel fontconfig-devel libzip-devel SDL2-devel zip speexdsp-devel ninja-build

WORKDIR /

RUN git clone https://github.com/emscripten-core/emsdk.git

WORKDIR /emsdk/

# Pin version - to prevent sudden breakage of the CI
RUN ./emsdk install 3.1.74
RUN ./emsdk activate 3.1.74

WORKDIR /openrct2/

COPY ./ ./

RUN rm -rf emscripten/temp/ emscripten/www/ emscripten/ext/

WORKDIR /emsdk/

RUN . ./emsdk_env.sh && cd /openrct2/emscripten/ && ./build_emscripten.sh

FROM scratch AS export

COPY --from=builder /openrct2/emscripten/www/* .
