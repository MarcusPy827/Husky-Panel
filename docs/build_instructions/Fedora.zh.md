# 在Fedora上构建HuskyPanel
> ⚠️ **严重警告**：Fedora 自带其构建的 Qt。混用不同版本的 Qt **会**导致 ABI 不匹配，可能损坏你的桌面会话，严重时甚至无法进入图形界面。**在安装任何依赖前，您应该手动执行一遍`sudo dnf upgrade --refresh`以升级所有软件包，您已经被警告过了。**

举个例子，一些Fedora Workstation，如FC43的出厂Qt版本是6.9, 你需要装devel包，从仓库上下载的版本往往是6.10+，但是DNF不会自动帮你更新剩下的旧Qt工具链，所以ABI不匹配了...

怎么解决呢？只需要手动执行一遍`sudo dnf upgrade --refresh`以升级所有软件包即可。

## 构建ECM（仅在 `USE_VENDORED_LIBS=ON` 时需要）

> **如果你使用的是系统的 `layer-shell-qt`（默认行为），可以跳过此步骤** —— 系统自带的 ECM 版本已经够用。

ECM (Extra CMake Modules) 被集成的 `layer-shell-qt` 所依赖。集成的版本需要非常新的 ECM，而很多非滚动发行版都无法满足最低版本要求。为此我们也集成了 ECM 模块。

**如果你需要传入 `-DUSE_VENDORED_LIBS=ON`**，请先按照如下说明来构建 ECM 模块：

首先，请打开终端，确保自己的终端上的位置在**项目根目录上**。

然后执行：
```bash
chmod a+x ./scripts/configure_ecm.sh
./scripts/configure_ecm.sh
```

运行脚本的时候脚本可能会询问一些问题，这些问题如下：

```bash
[WARN] The build directory "/XXXXX/husky-panel/build/ecm-build" is NOT found in your system.
Do you want to create it?\n    (y/n) >> 
```

> 也有可能询问的目录是`"/XXXXX/husky-panel/build/ecm-install"`，其与`ecm-build`的区别是前者是安装目录，后者是构建目录。

此问题报告：构建/安装目录不存在，是否创建？

首先，请检查问题打印出来的目录`"/XXXXXX/husky-panel/build/ecm-build"`是否就是`"<项目根目录>/build/ecm-build"`，如果不是则代表您的终端不在项目根目录上，输入`n`并按下回车取消执行脚本。

> 如果询问的是`"/XXXXX/husky-panel/build/ecm-install"`，那就是确认请检查问题打印出来的目录`"/XXXXXX/husky-panel/build/ecm-install"`是否就是`"<项目根目录>/build/ecm-install"`

> 输入`y`或`n`之外的任何内容都会打断脚本。

如果上一步中打印目录就是`"<项目根目录>/build/ecm-build"`，您需要输入`y`，然后按回车，脚本会自动创建这个构建路径。

<hr/>

``` bash
[WARN] The build directory "/XXXXXX/build/ecm-build" already exists and is NOT empty.
Do you want to clean it?\nI mean... to delete everything in it?\n    (y/n) >> 
```

> 也有可能询问的目录是`"/XXXXX/husky-panel/build/ecm-install"`，其与`ecm-build`的区别是前者是安装目录，后者是构建目录。

此问题报告：构建目录/安装已经存在且不为空，是否清空？
* 输入`y`，按下回车会清空这个目录下一切内容。
* 输入`n`，按下回车会跳过目录清理。
* 输入任何其他奇奇怪怪的东西也会跳过目录清理。

<hr/>

如果看到如下输出：
``` bash
[ OK ] ECM should be built and available in "/home/marcus/Desktop/Repository/Private/husky-panel/build/ecm-build/" now.
```

...则证明您的ECM模块已经构建好了。

## 安装依赖
```bash
sudo dnf install qt6-qtbase qt6-qtdeclarative qt6-qtwayland layer-shell-qt libdbusmenu-lxqt kf6-kservice pulseaudio-libs cmake gcc-c++ extra-cmake-modules qt6-qtbase-devel qt6-qtbase-private-devel qt6-qtwayland-devel qt6-qttools-devel layer-shell-qt-devel libdbusmenu-lxqt-devel kf6-kservice-devel wayland-devel wayland-protocols-devel libxkbcommon-devel pulseaudio-libs-devel git libxcb-devel

## 构建 RPM 软件包（推荐）

如果您希望通过包管理器来管理安装，可以构建并安装 RPM 包。请确保终端**处于项目根目录**。

首先安装打包工具：
```bash
sudo dnf install rpmdevtools rpm-build
```

创建源码压缩包并构建 RPM：
```bash
rpmdev-setuptree
APP_VERSION=1.0.0
tar czf ~/rpmbuild/SOURCES/husky-panel-${APP_VERSION}.tar.gz \
    --exclude='.git' \
    --transform "s,^\./,husky-panel-${APP_VERSION}/," .
cp packages/husky-panel.spec ~/rpmbuild/SPECS/
rpmbuild -ba ~/rpmbuild/SPECS/husky-panel.spec
```

安装构建好的软件包：
```bash
sudo dnf install ~/rpmbuild/RPMS/x86_64/husky-panel-*.rpm
```

## 构建与安装（不通过软件包）
### 构建面板自身
配置集成的库并构建面板：
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

此过程可能会持续数分钟...

### (可选) 安装到系统
```bash
sudo cmake --install .
```
> ⚠️ **注意**：当前HuskyPanel**不会**自动启动。要在登录时自动拉起HuskyPanel，请在您的桌面环境中手动为HuskyPanel配置自动启动。

#### (仅限KWin/Plasma) 安装KWin脚本
##### 如果你使用CMake安装了面板
KWin app-bridge 脚本插件会在执行 `cmake --install .`（见上方）时自动安装，默认通过 `INSTALL_KWIN_PLUGIN` CMake 选项启用。

如果你**不需要**此插件，可以在配置时传入 `-DINSTALL_KWIN_PLUGIN=OFF`。

#### 如果你没有通过CMake安装面板
您需要手动安装插件，请参阅 `plugins/kde/app-bridge` 中的 README。

安装完成后，打开*KDE 设置*，搜索「*KWin*」，在*KWin 脚本*栏中启用名为「*Husky-Panel App Bridge*」的脚本。你可能需要重启 KWin 或重新登录。

有关此插件的更多信息，请参阅 `plugins/kde/app-bridge` 中的 README。

## 注释
> ⚠️ **注意**: 此面板与Mutter不兼容，也就是无法在GNOME上运行此面板（X11上也许可以，但我们并不支持GNOME，也不会维护与GNOME相关的问题）

一些第三方库，例如abseil, gtest, material-color-utilities, qwindowkit会默认集成，无需单独安装。
