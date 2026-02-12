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

## Installation
```bash
cd ./plugins/kde/app-bridge
chmod a+x ./install.sh
./install.sh
```

Do NOT run as a super user.

请不要以超级用户身份安装。

After that, navigate to *KDE Settings*, Search "*KWin*", enable this KWin Script in the *KWin Script* section. Its name should be "*Husky-Panel App Bridge 0.9*", you may need a restart.

执行脚本后，打开*KDE设置*，搜索「*KWin*」，在*KWin脚本*栏启用此脚本。KWin脚本名称应为「*Husky-Panel App Bridge 0.9*」，您可能需要在完成后重启。

## Uninstallation
```bash
cd ./plugins/kde/app-bridge
chmod a+x ./uninstall.sh
./uninstall.sh
```

## License | 许可证
This script is licensed under GPL v3.

此脚本以GPL v3许可授权。

# Statement | 声明
KDE® and the K Desktop Environment® logo are registered trademarks of KDE e.V. We are not affiliated with, or endorsed by KDE e.V.

(Trademark Notice copied from https://techbase.kde.org/Template:KDE_Trademark_Notice)

KDE® 和 K Desktop Environment® 标志是 KDE e.V. 的注册商标。我们与 KDE e.V. 没有任何关联，也未获得其认可。
(商标声明源自：https://techbase.kde.org/Template:KDE_Trademark_Notice)
