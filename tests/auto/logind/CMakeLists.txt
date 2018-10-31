liri_add_test(tst_liri_logind
    SOURCES
        fakelogind.cpp
        fakelogind.h
        tst_logind.cpp
    LIBRARIES
        Liri::Logind
)
