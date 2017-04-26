%modules = ( # path to module name map
    "LiriCore" => "$basedir/src/core",
    "LiriDBusService" => "$basedir/src/dbusservice",
    "LiriNotifications" => "$basedir/src/notifications",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
    "desktopfile.h" => "Liri::DesktopFile",
    "desktopfileaction.h" => "Liri::DesktopFileAction",
    "dbusservice.h" => "Liri::DBusService",
    "notification.h" => "Liri::Notification",
);
