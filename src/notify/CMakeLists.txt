qt6_add_executable(liri-notify main.cpp)

target_compile_definitions(liri-notify PRIVATE -DLIBLIRI_VERSION="${PROJECT_VERSION}")

target_link_libraries(liri-notify PRIVATE Qt6::Core Liri::Notifications)

install(TARGETS liri-notify DESTINATION ${KDE_INSTALL_BINDIR})
