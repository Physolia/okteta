configure_file(oktetapart.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/oktetapart.desktop @ONLY)

add_library(oktetapart MODULE)

target_compile_definitions(oktetapart PRIVATE -DTRANSLATION_DOMAIN=\"oktetapart\")

target_link_libraries(oktetapart
    OktetaKastenControllers
    OktetaKastenGui
    OktetaKastenCore
    KastenControllers
    KastenGui
    KastenCore
    KF5::I18n
    KF5::Parts
)

target_sources(oktetapart
    PRIVATE
        part.cpp
        browserextension.cpp
        partfactory.cpp

        oktetapart.qrc
)

configure_file(partfactory-config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/partfactory-config.hpp @ONLY)

install( TARGETS oktetapart  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/parts)
kcoreaddons_desktop_to_json(oktetapart ${CMAKE_CURRENT_BINARY_DIR}/oktetapart.desktop)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/oktetapart.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
