# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(CoreQmlPlugin
    URI
        "Liri.Core"
    VERSION
        "1.0"
    GENERATE_PLUGIN_SOURCE
)

target_sources(CoreQmlPlugin
    PRIVATE
        formatter.cpp formatter.h
)

target_link_libraries(CoreQmlPlugin
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Liri::Core
)

ecm_finalize_qml_module(CoreQmlPlugin)
