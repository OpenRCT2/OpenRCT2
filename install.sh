#!/bin/bash

SDL2_PV=2.0.3

cachedir=.cache
mkdir -p $cachedir

echo `uname`

if [[ `uname` == "Darwin" ]]; then
    echo "Installation of OpenRCT2 assumes you have homebrew and use it to install packages."

    echo "Check if brew is installed"
    package_command="brew"
    which -s brew
    if [ $? -eq 1 ]; then
        echo "brew is not installed, or is not in your \$PATH"
        echo "Check if MacPorts is installed"
        which -s port
        if [ $? -eq 1 ]; then
            echo "MacPorts not found either, abort"
            exit
        else
            echo "MacPorts found"
            package_command="sudo port"
        fi
    else
        echo "brew was found"
    fi

    echo "Check if wget is installed"
    which -s wget
    if [ $? -eq 1 ]; then
        echo "wget is not installed, installing wget.."
        eval "$package_command install wget"
    fi

    # Install packages with whatever command was found.
    # Very possible I'm missing some dependencies here.
    eval "$package_command install cmake wine"

    if [[ ! -d /usr/include/wine ]]; then
        # This will almost certainly break as brew changes. Better ideas
        # welcome.
        wine_path="/usr/local/Cellar/wine/1.6.2/include/wine"
        if [ $package_command == "sudo port" ]; then
            wine_path="/opt/local/include/wine"
        fi
        sudo ln -s $wine_path /usr/include
    fi

    mingw_dmg=gcc-4.8.0-qt-4.8.4-for-mingw32.dmg
    mingw_path=/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin
    if [[ ! -f $cachedir/$mingw_dmg ]]; then
        wget http://crossgcc.rts-software.org/download/gcc-4.8.0-qt-4.8.4-win32/$mingw_dmg --output-document $cachedir/$mingw_dmg
    fi

    if [[ ! -d $mingw_path ]]; then
        echo "Open the DMG file and install its contents"
        open $cachedir/$mingw_dmg
    fi

    echo "You will need to add $mingw_path to your \$PATH"
elif [[ `uname` == "Linux" ]]; then
    sudo apt-get install -y --force-yes binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686
fi

if [[ ! -f $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz ]]; then 
    wget http://libsdl.org/release/SDL2-devel-${SDL2_PV}-mingw.tar.gz --output-document $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz; 
fi
if [[ ! -d $cachedir/SDL2-${SDL2_PV} ]]; then
    pushd $cachedir 
        tar -xzf SDL2-devel-${SDL2_PV}-mingw.tar.gz 
    popd
fi

# Apply platform patch
mingw_patch=libsdl2-mingw-2.0.3-fix-platform-detection-for-mingw.patch
if [[ ! -f $cachedir/$mingw_patch ]]; then 
    wget "https://github.com/anyc/anyc-overlay/raw/master/media-libs/libsdl2-mingw/files/$mingw_patch" --output-document $cachedir/$mingw_patch; 

    # XXX not sure how to make this idempotent.
    pushd $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32/include/SDL2/ 
        echo "Applying patch."
        patch -p2 < ../../../../$mingw_patch 
    popd
fi

if [[ ! -d /usr/local/cross-tools ]]; then
    sudo mkdir -p /usr/local/cross-tools
fi
if [[ ! -d /usr/local/cross-tools/i686-w64-mingw32 ]]; then
    sudo cp -r $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32 /usr/local/cross-tools/
fi

if [[ ! -f $cachedir/i686-w64-mingw32-pkg-config ]]; then
    # If this fails to work because of newlines, be sure you are running this
    # script with Bash, and not sh. We should really move this to a separate
    # file.
    echo -e "#!/bin/sh\nexport PKG_CONFIG_LIBDIR=/usr/local/cross-tools/i686-w64-mingw32/lib/pkgconfig\npkg-config \$@" > $cachedir/i686-w64-mingw32-pkg-config;
fi

chmod +x $cachedir/i686-w64-mingw32-pkg-config
sudo cp $cachedir/i686-w64-mingw32-pkg-config /usr/local/bin/

ls -al /usr/local/bin | grep pkg-config
cat /usr/local/bin/i686-w64-mingw32-pkg-config
