# Build OpenRCT2 using build environment image
FROM openrct2/openrct2:ubuntu_amd64 AS build-env
WORKDIR /openrct2
COPY . ./
RUN mkdir build \
 && cd build \
 && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/openrct2-install/usr -DCMAKE_INSTALL_LIBDIR=/openrct2-install/usr/lib \
 && ninja -k0 install

# Build runtime image
FROM ubuntu:18.04
RUN apt-get update
RUN apt-get -y upgrade
RUN apt-get install --no-install-recommends -y rsync ca-certificates libjansson4 libpng16-16 libzip4 libcurl4 libfreetype6 libfontconfig1

# Install OpenRCT2
COPY --from=build-env /openrct2-install /openrct2-install
RUN rsync -a /openrct2-install/* / \
 && rm -rf /openrct2-install \
 && openrct2-cli --version

# Done
ENTRYPOINT ["openrct2-cli"]
