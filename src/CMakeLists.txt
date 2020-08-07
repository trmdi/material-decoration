find_package (KDecoration2 REQUIRED)

find_package (Qt5 REQUIRED COMPONENTS
    Core
    Gui
)

find_package (KF5 REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    GuiAddons
    I18n
    IconThemes
    WindowSystem
)

# find_package(LibTaskManager)

find_package(X11 REQUIRED)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
    URL "http://www.x.org"
    TYPE REQUIRED
    PURPOSE "Required for building the X11 based workspace"
)

find_package(XCB MODULE REQUIRED COMPONENTS
    XCB
    RANDR
)
set_package_properties(XCB PROPERTIES TYPE REQUIRED)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS X11Extras)

if(X11_FOUND AND XCB_XCB_FOUND)
  set(HAVE_X11 ON)
endif()

# Discover KF5 version
string(REGEX MATCH "\\.([^]]+)\\." KF5_MINOR_VERSION ${KF5_VERSION})
string(REGEX REPLACE "\\." "" KF5_MINOR_VERSION ${KF5_MINOR_VERSION})
message(STATUS "KF5 VERSION: ${KF5_VERSION} (MINOR_VERSION=${KF5_MINOR_VERSION})")

# if(LibTaskManager_FOUND)
#     # Discover LibTaskManager version
#     string(REGEX MATCH "\\.([^]]+)\\." LibTaskManager_MINOR_VERSION ${LibTaskManager_VERSION})
#     string(REGEX REPLACE "\\." "" LibTaskManager_MINOR_VERSION ${LibTaskManager_MINOR_VERSION})
# else()
    # Initialize to not found
    set(LibTaskManager_CURRENTMINOR_VERSION -1)
# endif()
# message(STATUS "LibTaskManager VERSION: ${LibTaskManager_VERSION} (MINOR_VERSION=${LibTaskManager_MINOR_VERSION})")

configure_file(BuildConfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.h)

set (decoration_SRCS
    AppMenuModel.cc
    AppMenuButton.cc
    AppMenuButtonGroup.cc
    BoxShadowHelper.cc
    Button.cc
    Decoration.cc
    MenuOverflowButton.cc
    TextButton.cc
    ConfigurationModule.cc
    plugin.cc
)

kconfig_add_kcfg_files(decoration_SRCS
    InternalSettings.kcfgc
)

add_library (materialdecoration MODULE
    ${decoration_SRCS}
)

target_link_libraries (materialdecoration
    PUBLIC
        dbusmenuqt
        Qt5::Core
        Qt5::Gui
        Qt5::X11Extras
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::ConfigWidgets
        KF5::CoreAddons
        KF5::I18n
        KF5::GuiAddons
        KF5::IconThemes
        KF5::WindowSystem
        XCB::XCB

    PRIVATE
        KDecoration2::KDecoration
)

install (TARGETS materialdecoration
         DESTINATION ${PLUGIN_INSTALL_DIR}/org.kde.kdecoration2)
