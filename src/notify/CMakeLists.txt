liri_add_executable(liri-notify
    SOURCES
        main.cpp
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIBLIRI_VERSION="${PROJECT_VERSION}"
    LIBRARIES
        Qt5::Core
        Liri::Notifications
)

liri_finalize_executable(liri-notify)
