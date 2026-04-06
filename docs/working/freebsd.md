# FreeBSD (EXPERIMENTAL)

- FreeBSD 14.0+
- KDE Plasma 6 桌面环境
- Qt 6.5+
- Wayland 会话（推荐）或 EWMH 兼容的 X11 窗口管理器

```sh
sudo pkg install \
    cmake \
    pkgconf \
    wayland \
    wayland-protocols \
    libxkbcommon \
    qt6-base \
    qt6-wayland \
    qt6-tools \
    kf6-extra-cmake-modules \
    kf6-kservice \
    layer-shell-qt6 \
    libdbusmenu-lxqt
```

```sh
sudo pkg install libxcb
```

### (If use system lib)
```sh
git clone <repo-url>
cd husky-panel

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

### (If use vendored)
```sh
chmod a+x ./scripts/configure_ecm.sh
./scripts/configure_ecm.sh
```

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_VENDORED_LIBS=ON ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

### Install

```sh
sudo cmake --install .
```
