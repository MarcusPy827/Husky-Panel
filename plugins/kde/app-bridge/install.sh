kpackagetool6 --type=KWin/Script -i ./
kwriteconfig6 --file kwinrc --group Plugins --key marcus-app-bridgeEnabled true
qdbus org.kde.KWin /KWin reconfigure
