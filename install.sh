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

    mingw_name=mingw-w32-bin_i686-darwin
    mingw_tar=$mingw_name"_20130531".tar.bz2
    mingw_path=/usr/local/$mingw_name
    if [[ ! -f $cachedir/$mingw_tar ]]; then
        wget "https://downloads.sourceforge.net/project/mingw-w64/Toolchains targetting Win32/Automated Builds/$mingw_tar" --output-document $cachedir/$mingw_tar
    fi
    if [[ ! -d $ming_path ]]; then

        pushd /usr/local/
            sudo mkdir $mingw_name
        popd

        echo "Extracting contents of $mingw_tar to $mingw_path"
        echo "Don't forget to add $mingw_path/bin to your PATH variable!"
        sudo tar -xyf $cachedir/$mingw_tar -C $mingw_path

        pushd /usr/local
            sudo chmod 755 $mingw_name
            pushd $mingw_name
                sudo find . -type d -exec chmod 755 {} \;
            popd
        popd
    fi
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
