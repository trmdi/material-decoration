find_package (KDecoration2 REQUIRED)

find_package (Qt5 REQUIRED COMPONENTS
    Core
    Gui
)

find_package (KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    GuiAddons
    WindowSystem
)

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

set (decoration_SRCS
    AppIconButton.cc
    AppMenuModel.cc
    AppMenuButtonGroup.cc
    ApplicationMenuButton.cc
    BoxShadowHelper.cc
    CloseButton.cc
    CommonButton.cc
    CommonToggleButton.cc
    Decoration.cc
    KeepAboveButton.cc
    KeepBelowButton.cc
    MaximizeButton.cc
    MenuOverflowButton.cc
    MinimizeButton.cc
    OnAllDesktopsButton.cc
    TextButton.cc
    plugin.cc
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
        KF5::CoreAddons
        KF5::GuiAddons
        KF5::WindowSystem
        XCB::XCB

    PRIVATE
        KDecoration2::KDecoration
)

install (TARGETS materialdecoration
         DESTINATION ${PLUGIN_INSTALL_DIR}/org.kde.kdecoration2)
