# Husky-Panel App Bridge
## Description | 介绍
This is a simple KWin Script that reads the current real time application name, package name, and window title of the toplevel window. These data will be shared locally with the panel via D-Bus. Every application that subscribe its D-Bus is able to get those information. Use at your own risk.

这是一个简单的KWin脚本，会实时读取当前窗口的程序名，包名与窗体名。这些数据会在本地通过D-Bus与panel共享。所有订阅此D-Bus信号的程序都可以获得上述信息，请在知晓风险后再决定是否使用。

## Usage | 用途
This is required by the component "application indicator".

这个脚本被「应用程序指示器」依赖。

## Note | 注释
Plasma 6 is **REQUIRED** and Plasma 5 is **UNSUPPORTED**!!

此脚本**需要Plasma 6**，**不支持Plasma5**！！

## Installation | 安装
This plugin is installed automatically when you run `sudo cmake --install .` from the project build directory. It is controlled by the `INSTALL_KWIN_PLUGIN` CMake option (ON by default).

此插件会在项目构建目录下运行 `sudo cmake --install .` 时自动安装，由 `INSTALL_KWIN_PLUGIN` CMake 选项控制（默认开启）。

After installation, navigate to *KDE Settings*, search "*KWin*", and enable the script named "*Husky-Panel App Bridge*" in the *KWin Script* section. You may need to restart KWin or log out and back in.

安装完成后，打开*KDE 设置*，搜索「*KWin*」，在*KWin 脚本*栏中启用名为「*Husky-Panel App Bridge*」的脚本。您可能需要重启 KWin 或重新登录。

## Uninstallation | 卸载
Remove the installed files:

删除已安装的文件：
```bash
sudo rm -rf /usr/share/kwin/scripts/marcus-app-bridge
```

## License | 许可证
This script is licensed under GPL v3.

此脚本以GPL v3许可授权。

# Statement | 声明
KDE® and the K Desktop Environment® logo are registered trademarks of KDE e.V. We are not affiliated with, or endorsed by KDE e.V.

(Trademark Notice copied from https://techbase.kde.org/Template:KDE_Trademark_Notice)

KDE® 和 K Desktop Environment® 标志是 KDE e.V. 的注册商标。我们与 KDE e.V. 没有任何关联，也未获得其认可。
(商标声明源自：https://techbase.kde.org/Template:KDE_Trademark_Notice)
