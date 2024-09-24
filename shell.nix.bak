with import <nixpkgs> { };
(

  let
    python =
      let
        packageOverrides = self:
          super: {
            opencv4 = super.opencv4.override {
              enableGtk3 = true;
              gtk3 = pkgs.gtk3;
              enableFfmpeg = true; #here is how to add ffmpeg and other compilation flags
            };
          };
      in
      pkgs.python311.override { inherit packageOverrides; self = python; };

    opencvGtk = opencv.override (old: { enableGtk3 = true; });
  in

  stdenv.mkDerivation {
    name = "impurePythonEnv";
    buildInputs = [
      pkgs.clang-tools
      pkgs.clang

      pkgs.gdb
      pkgs.gnumake

      opencvGtk
      pkgs.pkg-config
      pkgs.libjpeg
      pkgs.libpng
      pkgs.libtiff
      pkgs.gtk3
      pkgs.libv4l
      pkgs.python3

      (python311.buildEnv.override {
        extraLibs = [
          pkgs.python311Packages.matplotlib
          pkgs.python311Packages.numpy
          pkgs.python311Packages.scipy
          pkgs.python311Packages.gnureadline
          pkgs.python311Packages.scikit-image
          python.pkgs.opencv4
        ];
        ignoreCollisions = true;
      })
    ];
    shellHook = ''
      SOURCE_DATE_EPOCH=$(date +%s)
      export LANG=en_US.UTF-8	

      echo 'Compile with make'
    '';
  }
)
