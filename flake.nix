{
  description = "Computer Vision based analytics for professional League of Legends";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    commonDependencies = [
      pkgs.cmake
      pkgs.clang
      pkgs.clang-tools
      pkgs.pkg-config
      pkgs.gnumake
      pkgs.gtk3
      pkgs.libjpeg
      pkgs.libpng
      pkgs.libtiff
    ];

    opencvGtk = pkgs.opencv4.override {
      enableGtk3 = true;
      gtk3 = pkgs.gtk3;
      enableFfmpeg = true;
    };

  in
  rec {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = commonDependencies ++ [
        pkgs.gdb
        pkgs.libv4l
        pkgs.python311
        (pkgs.python311.buildEnv.override {
          extraLibs = [
            pkgs.python311Packages.matplotlib
            pkgs.python311Packages.numpy
            pkgs.python311Packages.scipy
            pkgs.python311Packages.gnureadline
            pkgs.python311Packages.scikit-image
            opencvGtk
          ];
          ignoreCollisions = true;
        })
      ];

      shellHook = ''
        SOURCE_DATE_EPOCH=$(date +%s)
        export LANG=en_US.UTF-8	
      '';
    };

    opencv-lol = pkgs.stdenv.mkDerivation {
      name = "opencv-lol";
      nativeBuildInputs = commonDependencies ++ [ opencvGtk ];

      src = ./.;

      buildPhase = ''
        cmake . -DCMAKE_BUILD_TYPE=Release
        make -j $NIX_BUILD_CORES
      '';

      installPhase = ''
        mkdir -p $out/bin
        echo $(ls --all)
        cp opencv_lol $out/bin
      '';

    };

    defaultPackage.${system} = opencv-lol;

    defaultApp.${system} = flake-utils.lib.mkApp {
      drv = opencv-lol;
    };
  };
}
