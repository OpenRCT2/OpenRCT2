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
pkgs.stdenv.mkDerivation {
  name = "openrct2";
  src = builtins.filterSource
    (path: type:
      baseNameOf path != ".git" &&
      baseNameOf path != "output" &&
      baseNameOf path != "result")
    ./.;
  buildInputs = [
	pkgs.gcc
	pkgs.SDL2
	pkgs.freetype
	pkgs.fontconfig
	pkgs.libzip
	pkgs.speexdsp
	pkgs.curl
	pkgs.jansson
	pkgs.openssl
	pkgs.icu
	pkgs.zlib
	pkgs.libGL
	pkgs.cmake
	pkgs.pkgconfig
	pkgs.xorg.libpthreadstubs
] ++ (pkgs.stdenv.lib.optionals pkgs.stdenv.isDarwin [
    pkgs.Foundation # osX hacks
    pkgs.libobjc ]
  );
}
