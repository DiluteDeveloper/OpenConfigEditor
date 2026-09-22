{ pkgs ? import <nixpkgs> {} }:
    pkgs.mkShell {
        buildInputs = with pkgs.buildPackages; [ 
        qt6.qtbase
        qt6.qtbase.dev
        bear
        ];

    shellHook = ''
        build() {
            mkdir -p build && cd build
            qmake ../qt.pro
            cd ..
            bear -- make -C build
        }
    '';
}

