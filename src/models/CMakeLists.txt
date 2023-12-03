# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

liri_add_module(Models
    SOURCES
        mergeproxymodel.cpp mergeproxymodel.h mergeproxymodel_p.h
        qobjectlistmodel.cpp qobjectlistmodel.h
        qquicklist.h
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::Qml
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6Qml
)

liri_finalize_module(Models)
