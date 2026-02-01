# Husky-Panel
> 警告！！
> 此项目仅适配Wayland，完全不支持X11.

> WARNING!!
> This project is for Wayland ONLY, it does not work with X11.

## 依赖 | Dependency
在OpenSUSE上：
On OpenSUSE: 

```bash
# 注意：请使用Qt6以及Plasma 6
# Note: Do use Qt6 and Plasma 6
sudo zypper in wayland-devel \
  wayland-protocols-devel \
  kf6-extra-cmake-modules \
  libxkbcommon-devel
```

在Archlinux上：
On Archlinux: 
```bash
# 注意：请使用Qt6以及Plasma 6
# Note: Do use Qt6 and Plasma 6
sudo pacman -S wayland wayland-protocols extra-cmake-modules libxkbcommon
```

托了KDE的福，我们已经在`lib/3rdparty/layer-shell-qt`集成了`Layershell-Qt`库！！

Luckily, the `Layershell-Qt` library has been vendored to `lib/3rdparty/layer-shell-qt`, thanks KDE for the awsome library!!

另外，调试时请确保自己在Wayland会话下，本程序在X11下不起作用。

By the way, do ensure you are in a Wayland session while debuging. This program does NOT work under X11.
