# 在GXDE上安装HuskyPanel后需要做的事
您可能会发现应用程序菜单是空的，要解决这个问题，需要先安装一个包：

```bash
apt install gnome-menus
```

（这个包理论上上随此程序一起安装了）

并且运行如下指令

```bash
ls /etc/xdg/menus
```

如果您看到了`gnome-applications.menu`但没有`applications.menu`，只需要创建一个软链接即可：

```bash
sudo ln -s /etc/xdg/menus/gnome-applications.menu /etc/xdg/menus/applications.menu
```

重启面板，应用程序菜单应该能正常工作了。
