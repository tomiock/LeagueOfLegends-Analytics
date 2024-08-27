{
  description = "Computer Vision based analytics for professional League of Legends";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    mini-compile-commands = {
      url = "github:danielbarter/mini_compile_commands";
      flake = false;
    };
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, mini-compile-commands }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    commonDependencies = [
      pkgs.clang-tools
      pkgs.clang

      pkgs.cmake
      pkgs.pkg-config
      pkgs.gnumake
      pkgs.gdb

      #pkgs.opencv4

      pkgs.gtk3
      pkgs.libjpeg
      pkgs.libpng
      pkgs.libtiff
      pkgs.libv4l
    ];

    opencvGtk = pkgs.opencv4.override {
      enableGtk3 = true;
      gtk3 = pkgs.gtk3;
      enableFfmpeg = true;
    };

    opencv-lol = pkgs.stdenv.mkDerivation {
      name = "opencv-lol";

      nativeBuildInputs = commonDependencies ++ [ opencvGtk ];

      src = ./.;

      buildPhase = ''
        make
      '';

      installPhase = ''
        mkdir -p $out/bin
        cp opencv_lol $out/bin
        cp compile_commands.json $out
      '';

      postBuild = ''
        mkdir -p $out/
        cp compile_commands.json $out/compile_commands.json
      '';

    };

  in
  rec {
    devShells.${system}.default =
      with import nixpkgs { system = "x86_64-linux"; };
      let 
          mcc-env = (callPackage mini-compile-commands {}).wrap stdenv;
      in (pkgs.mkShell.override {stdenv = mcc-env;}) {

      buildInputs = commonDependencies ++ [

        opencvGtk
        pkgs.opencv

        pkgs.python3
        (python311.buildEnv.override {
          extraLibs = [
            pkgs.python311Packages.matplotlib
            pkgs.python311Packages.numpy
            pkgs.python311Packages.scipy
            pkgs.python311Packages.gnureadline
            pkgs.python311Packages.scikit-image
                  #python.pkgs.opencv4
          ];
          ignoreCollisions = true;
        })
      ];

      shellHook = ''
        SOURCE_DATE_EPOCH=$(date +%s)
        export LANG=en_US.UTF-8	
      '';
    };

    defaultPackage.${system} = opencv-lol;

    defaultApp.${system} = flake-utils.lib.mkApp {
      drv = opencv-lol;
    };
  };
}
