{ pkgs ? (
   let 
    hostPkgs = import <nixpkgs> {};
    pinnedPkgs = hostPkgs.fetchFromGitHub {
      owner = "NixOS";
      repo = "nixpkgs-channels";
      rev = "08d245eb31a3de0ad73719372190ce84c1bf3aee";
      sha256 = "1g22f8r3l03753s67faja1r0dq0w88723kkfagskzg9xy3qs8yw8";
    };
  in
  import pinnedPkgs {}
 )
}:
let
  objects-src = pkgs.fetchFromGitHub {
    owner = "OpenRCT2";
    repo = "objects";
    rev = "v1.0.10";
    sha256 = "4f261964f1c01a04b7600d3d082fb4d3d9ec0d543c4eb66a819eb2ad01417aa0";
  };

  title-sequences-src = pkgs.fetchFromGitHub {
    owner = "OpenRCT2";
    repo = "title-sequences";
    rev = "v0.1.2";
    sha256 = "1yb1ynkfmiankii3fngr9km5wbc07rp30nh0apkj6wryrhy7imgm";
};
in
pkgs.stdenv.mkDerivation {
  name = "openrct2";
  src = ./.;

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=RELWITHDEBINFO"
    "-DDOWNLOAD_OBJECTS=OFF"
    "-DDOWNLOAD_TITLE_SEQUENCES=OFF"
  ];

  postUnpack = ''
    cp -r ${objects-src}         $sourceRoot/data/object
    cp -r ${title-sequences-src} $sourceRoot/data/title
  '';
  preFixup = "ln -s $out/share/openrct2 $out/bin/data";


  makeFlags = ["all" "g2"];

  buildInputs = [
	pkgs.SDL2
	pkgs.cmake
	pkgs.curl
	pkgs.fontconfig
	pkgs.freetype
	pkgs.icu
	pkgs.jansson
  pkgs.libiconv
  pkgs.libpng
	pkgs.libGLU
	pkgs.libzip
	pkgs.openssl
	pkgs.pkgconfig
	pkgs.speexdsp
	pkgs.xorg.libpthreadstubs
	pkgs.zlib
] ++ (pkgs.stdenv.lib.optionals pkgs.stdenv.isDarwin [
    pkgs.darwin.apple_sdk.frameworks.Foundation # osX hacks
    pkgs.darwin.apple_sdk.frameworks.AppKit
    pkgs.darwin.libobjc ]
  );
}
