# Building Instruction: Fedora
## Building ECM (only needed when `USE_VENDORED_LIBS=ON`)

> **If you are using system `layer-shell-qt` (the default option), you can skip this step entirely** — the system ECM is sufficient.

ECM (Extra CMake Modules) is required by the vendored `layer-shell-qt`. The vendored version depends on a very new ECM, and many non-rolling distros do not meet the minimum version requirement.

To solve this, we have vendored the ECM module as well. **If you pass `-DUSE_VENDORED_LIBS=ON`**, please build the ECM module first:

First, open a terminal, make sure you are **on project root**.

Then execute the following: 
```bash
chmod a+x ./scripts/configure_ecm.sh
./scripts/configure_ecm.sh
```

If you see the log output:
```bash
[ OK ] ECM should be built and available in "/home/marcus/Desktop/Repository/Private/husky-panel/build/ecm-build/" now.
```

... then ECM modules are ready to use for next steps!!

## Install Dependencies

**Runtime dependencies:**
```bash
sudo pacman -S qt6-base qt6-wayland qt6-declarative layer-shell-qt kservice libdbusmenu-lxqt libpulse cmake extra-cmake-modules qt6-tools wayland wayland-protocols libxkbcommon git libxcb
```

## Build as Arch Package (Recommended)
If you prefer a proper system-managed installation, you can build and install a `.pkg.tar.zst` package using `makepkg`:

```bash
cd packages
makepkg -si
```

The package will be managed by `pacman`, so it can be removed cleanly with `sudo pacman -R husky-panel`.

## Build & Installation (Skipping Building The `.pkg.tar.zst` Package)
### Build the Bar Itself
Configure with vendored libraries, then build:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The process could last for minutes...

### (Optional) Install to System

```bash
sudo cmake --install .
```

> **Note**: Currently HuskyPanel does NOT autostart itself after you login, you may want to add it to your desktop environment's autostart settings MANUALLY.

### (KWin Only) Install the Plugin
#### If You Have Runned the CMake Install
You may ignore this section. The plugin should be installed already w/ CMake.

The KWin app-bridge script plugin is installed automatically by `cmake --install .`. It is enabled by default via the `INSTALL_KWIN_PLUGIN` CMake option.

If you do **not** want the plugin, pass `-DINSTALL_KWIN_PLUGIN=OFF` at configure time.

If the plugin does not work because it is the first time you install, simply restart your device.

#### If You Haven't Runned the CMake Install Yet
You will need install the plugin manually. Please turn to the README in `plugins/kde/app-bridge`.

After installation, navigate to *KDE Settings*, search "*KWin*", and enable the script named "*Husky-Panel App Bridge*" in the *KWin Script* section. You may need to restart KWin or log out and back in.

For more details about the plugin, read the README in `plugins/kde/app-bridge`.

## Side Notes
> ⚠️ **Note**: This panel is NOT compatible with Mutter, hence there is no way to run this panel on GNOME. (On X11 session it may, but we do not officially support GNOME).

Some third-party libraries (abseil, gtest, material-color-utilities, qwindowkit) are always vendored and do NOT need to be installed separately.
