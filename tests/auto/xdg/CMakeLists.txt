# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

qt6_add_executable(tst_liri_xdg tst_desktopfile.cpp)

target_link_libraries(tst_liri_xdg PRIVATE Qt6::Test Liri::Xdg)

add_test(
    NAME tst_liri_xdg
    COMMAND tst_liri_xdg
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
