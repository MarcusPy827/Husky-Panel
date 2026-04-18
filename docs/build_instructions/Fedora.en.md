# Building Instruction: Fedora
> ⚠️ **WARNING**: Fedora ships its own Qt build. Mixing Qt versions **will** cause ABI mismatches, which can corrupt your desktop session and potentially make your system unbootable into a graphical environment. Before installing dependency you will have run `sudo dnf upgrade --refresh` manually. YOU HAVE BEEN WARNED.

For example, some Fedora Workstation, e.g. FC43 ships with Qt 6.9, your devel package downloaded from the repo will be 6.10+, this will cause a ABI mismatch and DNF will NOT upgrade the rest of the Qt toolchain for you...

How to resolve it? Simply upgrade your system's softwares with `sudo dnf upgrade --refresh`.

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
sudo dnf install qt6-qtbase qt6-qtdeclarative qt6-qtwayland layer-shell-qt libdbusmenu-lxqt kf6-kservice pulseaudio-libs cmake gcc-c++ extra-cmake-modules qt6-qtbase-devel qt6-qtbase-private-devel qt6-qtwayland-devel qt6-qttools-devel layer-shell-qt-devel libdbusmenu-lxqt-devel kf6-kservice-devel wayland-devel wayland-protocols-devel libxkbcommon-devel pulseaudio-libs-devel git libxcb-devel
```

## Build as RPM Package (Recommended)
If you prefer a proper system-managed installation, you can build and install an RPM package. Make sure you are **on the project root**.

First, install following packaging tools:
```bash
sudo dnf install rpmdevtools rpm-build
```

Then create the source tarball and build the RPM:
```bash
rpmdev-setuptree
APP_VERSION=1.0.0
tar czf ~/rpmbuild/SOURCES/husky-panel-${APP_VERSION}.tar.gz \
    --exclude='.git' \
    --transform "s,^\./,husky-panel-${APP_VERSION}/," .
cp packages/husky-panel.spec ~/rpmbuild/SPECS/
rpmbuild -ba ~/rpmbuild/SPECS/husky-panel.spec
```

Then install the built package:
```bash
sudo dnf install ~/rpmbuild/RPMS/x86_64/husky-panel-*.rpm
```

## Build & Installation (Skipping Building The RPM Package)
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
