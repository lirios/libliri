liri_add_module(LocalDevice
    DESCRIPTION
        "Operations on local device such as reboot, power off, suspend, etc..."
    SOURCES
        localdevicebackend_p.cpp localdevicebackend_p_p.h
        localdevice.cpp localdevice.h localdevice_p.h
        osrelease.cpp osrelease.h osrelease_p.h
        backends/systemdbackend.cpp backends/systemdbackend_p.h
        backends/upowerbackend.cpp backends/upowerbackend_p.h
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::DBus
        Qt6::QmlIntegration
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6DBus
)

liri_finalize_module(LocalDevice)
