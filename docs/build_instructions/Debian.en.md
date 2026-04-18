# Building Instruction: Debian-Based Distros

> ⚠️ **Note**: For Debian, make sure that your Debian version is Trixie.

> ℹ️ **Note**: For GXDE users, we have tested on GXDE 25. Except some session options are missing on the default Desktop environment but the building shall be fine. You will have to use the command `QT_QPA_PLATFORMTHEME=QT_QUICK_CONTROLS_STYLE=Basic HuskyPanel ` to launch the panel.

> ℹ️ **Note**: Seems that Debian does NOT ship the `libdbusmenu-lxqt` development packages. Luckily, we have already vendored it. Use the flag `-DDEBIAN_PRESET=ON` while building.

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

```bash
sudo sudo apt install build-essential cmake \
    libwayland-dev wayland-protocols libwayland-bin libxkbcommon-dev pkg-config \
    qt6-base-dev qt6-base-private-dev \
    qt6-declarative-dev qt6-declarative-private-dev \
    qt6-wayland-dev qt6-wayland-private-dev qt6-wayland-dev-tools \
    qt6-tools-dev qtcreator \
    libkf6service-dev extra-cmake-modules \
    liblayershellqtinterface-dev \
    libpulse-dev
```

## Build & Installation
### Build the Bar Itself

Configure with vendored libraries, then build:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DDEBIAN_PRESET=ON ..
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
