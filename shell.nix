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
      pkgs.gcc
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
      # set SOURCE_DATE_EPOCH so that we can use python wheels
      SOURCE_DATE_EPOCH=$(date +%s)
      export LANG=en_US.UTF-8	
    '';
  }
)
/*



  { pkgs ? import <nixpkgs> {} }:

  pkgs.mkShell {
  buildInputs = [
  pkgs.cmake
  pkgs.python3
  pkgs.gcc
  pkgs.git
  pkgs.pkg-config
  pkgs.ffmpeg
  pkgs.libjpeg
  pkgs.libpng
  pkgs.libtiff
  pkgs.gtk3
  pkgs.libv4l
  pkgs.gstreamer
  pkgs.gst-plugins-base
  pkgs.opencv4
  ];

  shellHook = ''
  echo "Development environment for OpenCV application is ready."
  echo "Use the following command to compile your program:"
  echo "g++ -o process_video process_video.cpp $(pkg-config --cflags --libs opencv4)"
  '';
  }





  # shell.nix
  let
  # We pin to a specific nixpkgs commit for reproducibility.
  # Last updated: 2024-04-29. Check for new commits at https://status.nixos.org.
  pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/refs/tags/24.05.tar.gz") { };
  in
  pkgs.mkShell {
  packages = [
  (pkgs.python3.withPackages (python-pkgs: [
  # select Python packages here
  python-pkgs.scikit-image
  #python-pkgs.opencv4
  #pkgs.python312Packages.opencv4
  (python-pkgs.opencv4.override { enableGtk2 = true; })
  ]))
  pkgs.opencv4
  pkgs.gcc
  pkgs.pkg-config
  ];
  }
*/
