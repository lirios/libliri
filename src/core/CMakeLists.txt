# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

liri_add_module(Core
    DESCRIPTION
        "Utilities for Liri apps"
    SOURCES
        kformat.cpp kformat.h
        kformatprivate.cpp kformatprivate_p.h
    PUBLIC_LIBRARIES
        Qt6::Core
    PKGCONFIG_DEPENDENCIES
        Qt6Core
)

liri_finalize_module(Core)
