set(krazy2settings_cfg_SRCS
    krazy2preferences.cpp
)

set(krazy2settings_cfg_UI
    krazy2settings.ui
)

kde4_add_ui_files(krazy2settings_cfg_SRCS ${krazy2settings_cfg_UI})
kde4_add_kcfg_files(krazy2settings_cfg_SRCS krazy2config.kcfgc)
kde4_add_plugin(kcm_kdev_krazy2settings ${krazy2settings_cfg_SRCS})
target_link_libraries(kcm_kdev_krazy2settings ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS})

install(TARGETS kcm_kdev_krazy2settings DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kcm_kdev_krazy2settings.desktop DESTINATION ${SERVICES_INSTALL_DIR})
