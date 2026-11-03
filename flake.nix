# SPDX-FileCopyrightText: 2026 Jiffly
# SPDX-License-Identifier: MIT OR Apache-2.0

{
  description = "Meowdia";

  inputs = {
    crane.url = "github:ipetkov/crane";
    fenix = {
      url = "github:nix-community/fenix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "https://channels.nixos.org/nixos-26.05/nixexprs.tar.xz";
  };

  outputs =
    {
      self,
      crane,
      fenix,
      flake-utils,
      nixpkgs,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ fenix.overlays.default ];
        };
        rustToolchain = pkgs.fenix.stable.withComponents [
          "cargo"
          "rustc"
          "rustfmt"
          "clippy"
        ];
        craneLib = (crane.mkLib pkgs).overrideToolchain rustToolchain;

        commonArgs = {
          src = pkgs.lib.fileset.toSource {
            root = ./.;
            fileset = pkgs.lib.fileset.unions [
              (craneLib.fileset.commonCargoSources ./.)
            ];
          };
          strictDeps = true;
        };

        cargoArtifacts = craneLib.buildDepsOnly (
          commonArgs
          // {
            cargoExtraArgs = "--workspace";
            pname = "meowdia";
          }
        );

        cargoArtifactsDev = cargoArtifacts.overrideAttrs (
          final: prev: {
            CARGO_PROFILE = "dev";
          }
        );

        meowdiaClippy = craneLib.cargoClippy (
          commonArgs
          // {
            CARGO_PROFILE = "dev";
            cargoArtifacts = cargoArtifactsDev;
            cargoClippyExtraArgs = "--all-targets -- --deny warnings";
          }
        );

        meowdiaFmt = craneLib.cargoFmt commonArgs;

        meowdiaTest = craneLib.cargoTest (
          commonArgs
          // {
            CARGO_PROFILE = "dev";
            cargoArtifacts = cargoArtifactsDev;
          }
        );

        meowdia = craneLib.buildPackage (
          commonArgs
          // {
            inherit cargoArtifacts;
          }
        );

        cSdkSupported = pkgs.stdenv.hostPlatform.isLinux && pkgs.stdenv.hostPlatform.is64bit;

        cSource = pkgs.lib.fileset.toSource {
          root = ./.;
          fileset = pkgs.lib.fileset.unions [
            ./Makefile
            ./.clang-format
            ./.clang-tidy
            ./include
            ./sdk/c
            ./tests/abi/c
            ./LICENSES
          ];
        };

        mkCSdk = { clang ? false, sanitize ? false, lint ? false }:
          let
            compiler = if clang then pkgs.llvmPackages_21.clang else pkgs.gcc;
          in
          pkgs.stdenv.mkDerivation {
            pname = "meow-c-sdk";
            version = "0.1.0";
            src = cSource;
            strictDeps = true;
            nativeBuildInputs = pkgs.lib.optionals lint [ pkgs.llvmPackages_21.clang-tools ]
              ++ [ pkgs.gnumake compiler ];
            makeFlags = [
              "CC=${compiler}/bin/${if clang then "clang" else "gcc"}"
              "PREFIX=$(out)"
              "SANITIZE=${if sanitize then "1" else "0"}"
            ];
            doCheck = true;
            checkTarget = if lint then [ "check" "lint" ] else "check";
            meta.platforms = pkgs.lib.filter
              (platform: (pkgs.lib.systems.elaborate platform).is64bit)
              pkgs.lib.platforms.linux;
          };

        cSdk = mkCSdk {};
        cClang = mkCSdk { clang = true; };
        cSanitize = mkCSdk { clang = true; sanitize = true; };
        cSdkLint = mkCSdk { clang = true; lint = true; };

      in
      {
        checks = {
          clippy = meowdiaClippy;
          test = meowdiaTest;
          fmt = meowdiaFmt;
          reuse =
            pkgs.runCommand "meowdia-reuse"
              {
                src = ./.;
                nativeBuildInputs = [ pkgs.reuse ];
              }
              ''
                cd $src
                reuse lint
                touch $out
              '';
        } // pkgs.lib.optionalAttrs cSdkSupported {
          c-sdk = cSdk;
          c-clang = cClang;
          c-sanitize = cSanitize;
          c-sdk-lint = cSdkLint;
        };

        packages = {
          default = meowdia;
          ci_fmt = meowdiaFmt;
          ci_clippy = meowdiaClippy;
          ci_test = meowdiaTest;
        } // pkgs.lib.optionalAttrs cSdkSupported {
          c-sdk = cSdk;
          ci_c_sdk_lint = cSdkLint;
          ci_c_clang = cClang;
          ci_c_sanitize = cSanitize;
        };

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            rustToolchain
            just
            reuse
          ] ++ pkgs.lib.optionals cSdkSupported [
            pkgs.llvmPackages_21.clang-tools
            pkgs.gcc
            pkgs.llvmPackages_21.clang
            pkgs.gnumake
          ];
        };
      }
    );
}
