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
            odin
            nasm
            xxd
          ];

          shellHook = ''
            exec ${pkgs.zsh}/bin/zsh -c ${pkgs.zellij}/bin/zellij
          '';
        };
      };
    };
}
