# BetterInputs

A [Geode SDK](https://geode-sdk.org) mod for Geometry Dash (C++23) that improves `CCTextInputNode` behavior (text selection, hotkeys, etc.). It targets Windows and macOS; the build output is a `.geode` package (a zip containing the compiled platform binary plus `mod.json`, `about.md`, and `logo.png`).

Platform-specific source lives in `src/platform/windows.cpp` and `src/platform/macos.mm`; shared logic is under `src/`. The required Geode SDK version is pinned in `mod.json` (`geode` field).

## Cursor Cloud specific instructions

This is a Geometry Dash mod, not a runnable standalone app. Geometry Dash itself is proprietary and is NOT available in this environment, so "running" the mod means **building and packaging it into a `.geode` file**. There is no automated test suite and no linter config in this repo; CI (`.github/workflows/geode-build.yml`) only builds the mod.

### Building (Windows target, the only one buildable on Linux)

Geode supports cross-compiling **Windows** mods from Linux. macOS cannot be cross-compiled here (`geode build -p mac-os` reports "we do not know of any way to cross-compile to MacOS"); CI builds macOS on macOS runners.

From the repo root:

```bash
geode build -p windows
```

Output lands in `build-win/`: `spaghettdev.betterinputs.dll` and the packaged `spaghettdev.betterinputs.geode`. Incremental rebuilds take a couple seconds; a clean build (cold bindings + GeodeBindings codegen) takes a few minutes.

### Non-obvious environment notes

- **Required env vars** (exported in `~/.bash_profile` and `~/.bashrc`, so login/interactive shells get them automatically): `GEODE_SDK=/home/ubuntu/geode-sdk`, `LLVM_VER=19`, `CLANG_VER=19`. If you spawn a non-login, non-interactive shell that lacks them, `export` them manually or the build will fail (CMake errors out without `GEODE_SDK`, and the SDK requires Clang >= 19).
- The Geode SDK clone lives at `~/geode-sdk` (pinned to the version in `mod.json`). The Windows cross-compile toolchain (xwin Windows SDK "splat", `clang-msvc-sdk` CMake toolchain) lives at `~/.local/share/Geode/cross-tools/`. Prebuilt Geode loader binaries were installed via `geode sdk install-binaries -p win`.
- `LLVM_VER`/`CLANG_VER` must be `19` because the toolchain file (`~/.local/share/Geode/cross-tools/clang-msvc-sdk/clang-msvc.cmake`) searches for unversioned `clang`/`llvm-*` first (which default to the system's Clang 18); setting these forces it to pick the `*-19` binaries.
- CMake must be >= 3.29 for cross-compilation; this VM uses CMake 3.31 at `/usr/local/bin/cmake` (the distro's 3.28 at `/usr/bin/cmake` is too old).
- A dummy Geode profile (`ci-dummy`, pointing at `~/fake-gd`) exists only so the Geode CLI is happy; it is not a real Geometry Dash install. The build prints a harmless warning about the missing profile path when it tries to auto-install the packaged mod.
- The two `-Wunused-result` warnings from `src/platform/windows.cpp` are pre-existing and do not fail the build.
