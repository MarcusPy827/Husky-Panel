# Post Installation Manual for Running Husky Panel on GXDE
You may find out that the Application flyout not working as the list is empty. To fix this problem, you will need to first install a package: 

```bash
apt install gnome-menus
```

(This one shoule be installed together with this application)

And then run the following: 

```bash
ls /etc/xdg/menus
```

If you see `gnome-applications.menu` but not `applications.menu`, simply make a soft link: 

```bash
sudo ln -s /etc/xdg/menus/gnome-applications.menu /etc/xdg/menus/applications.menu
```

Then restart the panel, and the Application flyout shall be working.
