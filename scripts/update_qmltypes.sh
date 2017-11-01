#!/bin/bash

QMLPLUGINDUMP=${QMLPLUGINDUMP-qmlplugindump}

case $1 in
-h|--help)
    echo "Usage: $(basename $0) [IMPORT_PATH]"
    echo "it uses either '$(which qmlplugindump)' or the one set by 'QMLPLUGINDUMP'"
    exit 1
;;
esac

cmd="${QMLPLUGINDUMP} -noinstantiate -notrelocatable -platform minimal"
curpath=`dirname $0`
rootpath=`dirname $(readlink -e $curpath)`

$cmd Liri.Core 1.0 $1 > $rootpath/src/imports/core/plugins.qmltypes
$cmd Liri.Device 1.0 $1 > $rootpath/src/imports/device/plugins.qmltypes
$cmd Liri.Notifications 1.0 $1 > $rootpath/src/imports/notifications/plugins.qmltypes
