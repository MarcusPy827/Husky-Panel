console.info("marcus.panel.util.AppBridge is being loaded!!");

workspace.windowActivated.connect(function(window) {
  if (!window){
    return;
  }

  var name = window.resourceName ? window.resourceName.toString() : "Unknown";
  var appid = window.resourceClass ? window.resourceClass.toString() : "Unknown";
  var title = window.caption ? window.caption.toString() : "Unknown";

  console.log("marcus.panel.util.AppBridge Window has been changed into: "
    + name);

  callDBus("marcus.panel.util.AppBridge", "/AppBridge",
    "marcus.panel.util.AppBridge", "UpdateActiveApp", name, appid, title);
});
