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
    rev = "v1.2.2";
    sha256 = "2c77023068831c68423eb51f96251d1a06f58253414e4563ca17407d654ed96b";
  };

  title-sequences-src = pkgs.fetchFromGitHub {
    owner = "OpenRCT2";
    repo = "title-sequences";
    rev = "v0.1.2b";
    sha256 = "a220b4c3cd3180bebb261cc59141b7fb290b433631ba9c7587a4f2c9f7dc4e4c";
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
    cp -r ${title-sequences-src} $sourceRoot/data/sequence
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
