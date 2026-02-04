{
  description = "Homework solutions for the Computer Enhance programming series by Casey Muratori";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];
      perSystem = {
        config,
        self',
        inputs',
        pkgs,
        system,
        ...
      }: {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            gdb

            # NOTE including this package was the only way I could the missing `iostream`
            # file header warning to disappear. It isn't actually necessary to compile,
            # only for the LSP.
            gccNGPackages_15.libstdcxx

            go
            python3
            nasm
          ];

          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (with pkgs; [libcxx]);

          shellHook = ''
            exec zsh -c zellij
          '';
        };
      };
    };
}
