
cd "$(dirname "$0")"

START_DIR=$(pwd)
ICU_ROOT=$(pwd)/ext/icu/icu4c/source
JSON_DIR=/usr/include/nlohmann/

build_ext() {
	mkdir -p ext/
	cd ext/
	# Pin versions - to prevent sudden breakage
	if [ ! -d "speexdsp" ]; then
		git clone https://gitlab.xiph.org/xiph/speexdsp.git --depth 1 --branch SpeexDSP-1.2.1
        cd speexdsp
        cd ..
    fi
	if [ ! -d "icu" ]; then
		git clone https://github.com/unicode-org/icu.git --depth 1 --branch release-76-1
        cd icu
        cd ..
	fi
	if [ ! -d "libzip" ]; then
		git clone https://github.com/nih-at/libzip.git --depth 1 --branch v1.11.2
        cd libzip
        cd ..
	fi
	if [ ! -d "zlib" ]; then
		git clone https://github.com/madler/zlib.git --depth 1 --branch v1.3.1
        cd zlib
        cd ..
	fi
	if [ ! -d "vorbis" ]; then
		git clone https://gitlab.xiph.org/xiph/vorbis.git --depth 1 --branch v1.3.7
        cd vorbis
        cd ..
	fi
	if [ ! -d "ogg" ]; then
		git clone https://gitlab.xiph.org/xiph/ogg.git --depth 1 --branch v1.3.5
        cd ogg
        cd ..
	fi
	if [ ! -d "$JSON_DIR" ]; then
		echo "$JSON_DIR does not exist. Set in build_emscripten.sh or install the nlohmann-json headers!"
		exit 1
	fi
	rm -rf ../../src/thirdparty/nlohmann
	cp -r $JSON_DIR ../../src/thirdparty/nlohmann

	cd speexdsp
	emmake ./autogen.sh
	emmake ./configure --enable-shared --disable-neon
	emmake make -j$(nproc)
	cd $START_DIR/ext/

	cd icu/icu4c/source
	ac_cv_namespace_ok=yes icu_cv_host_frag=mh-linux emmake ./configure \
		--enable-release \
		--enable-shared \
		--disable-icu-config \
		--disable-extras \
		--disable-icuio \
		--disable-layoutex \
		--disable-tools \
		--disable-tests \
		--disable-samples
	emmake make -j$(nproc)
	cd $START_DIR/ext/

	cd zlib
	emcmake cmake ./
	emmake make zlib -j$(nproc)
	emmake make install
	ZLIB_ROOT=$(pwd)
	cd $START_DIR/ext/

	cd libzip
	mkdir -p build/
	cd build/
	emcmake cmake ../ -DZLIB_INCLUDE_DIR="$ZLIB_ROOT" -DZLIB_LIBRARY="$ZLIB_ROOT/libz.a"
	emmake make zip -j$(nproc)
	emmake make install
	cd $START_DIR/ext/

	cd ogg
	mkdir -p build/
	cd build/
	emcmake cmake ../
	emmake make -j$(nproc)
	emmake make install
	cd $START_DIR/ext/

	cd vorbis
	mkdir -p build/
	cd build/
	emcmake cmake ../
	emmake make -j$(nproc)
	emmake make install

	cd $START_DIR
}

build_assets() {
	mkdir temp/
	cd temp/
	cmake ../../ -DMACOS_BUNDLE=off -DDISABLE_NETWORK=on -DDISABLE_GUI=off
	make openrct2-cli -j$(nproc)
	make g2 -j$(nproc)
	DESTDIR=. make install
	mkdir -p ../static/assets/
	cp -r usr/local/share/openrct2/* ../static/assets/
	cd ../static/assets
	zip -r ../assets.zip *
	cd ../
	rm -rf assets/

	cd $START_DIR
}

if [ "$1" != "skip" ] ; then
build_ext
build_assets
fi

emcmake cmake ../ \
	-G Ninja \
	-DDISABLE_NETWORK=ON \
	-DDISABLE_HTTP=ON \
	-DDISABLE_TTF=ON \
	-DDISABLE_FLAC=ON \
	-DDISABLE_DISCORD_RPC=ON \
	-DCMAKE_SYSTEM_NAME=Emscripten \
	-DCMAKE_BUILD_TYPE=Release \
	-DSPEEXDSP_INCLUDE_DIR="$(pwd)/ext/speexdsp/include/" \
	-DSPEEXDSP_LIBRARY="$(pwd)/ext/speexdsp/libspeexdsp/.libs/libspeexdsp.a" \
	-DICU_INCLUDE_DIR="$ICU_ROOT/common" \
	-DICU_DATA_LIBRARIES=$ICU_ROOT/lib/libicuuc.so \
	-DICU_DT_LIBRARY_RELEASE="$ICU_ROOT/stubdata/libicudata.so" \
	-DLIBZIP_LIBRARIES="$(pwd)/ext/libzip/build/lib/libzip.a" \
	-DEMSCRIPTEN_FLAGS="-s USE_SDL=2 -s USE_BZIP2=1 -s USE_LIBPNG=1 -pthread -O3" \
	-DEMSCRIPTEN_LDFLAGS="-Wno-pthreads-mem-growth -s ASYNCIFY -s FULL_ES3 -s SAFE_HEAP=0 -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=4GB -s INITIAL_MEMORY=2GB -s MAX_WEBGL_VERSION=2 -s PTHREAD_POOL_SIZE=120 -pthread -s EXPORTED_RUNTIME_METHODS=FS,callMain,UTF8ToString,stringToNewUTF8 -lidbfs.js --use-preload-plugins -s MODULARIZE=1 -s 'EXPORT_NAME=\"OPENRCT2_WEB\"'"

emmake ninja -j$(nproc)

rm -rf www/
mkdir -p www/
cd www/
cp -r ../openrct2.* ./
cp -r ../static/* ./
cp -r ../static/.* ./

echo "finished!"
