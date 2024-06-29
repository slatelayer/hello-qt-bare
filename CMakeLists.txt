
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0140 NEW)

project(slater VERSION 0.1 LANGUAGES C CXX)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup()

qt_add_executable(slater
    main.cpp
)

qt_add_qml_module(slater
    URI slater
    RESOURCE_PREFIX /
    VERSION 1.0
    QML_FILES ui/Main.qml
)

qt_add_resources(slater "js"
    FILES
        ui/core.mjs
        ui/js/encoding.mjs
)

target_link_libraries(slater
    PRIVATE
        Qt6::Quick
    PUBLIC
    $<LINK_LIBRARY:WHOLE_ARCHIVE,bare_static>
)

set_target_properties(slater PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER local.slater
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(slater
  PRIVATE
  $<TARGET_PROPERTY:bare,INTERFACE_INCLUDE_DIRECTORIES>
  vendor/concurrentqueue
)

include(bare)
bare_target(target)
add_subdirectory(vendor/bare EXCLUDE_FROM_ALL)

link_bare_modules(slater AMALGAMATE)

include(GNUInstallDirs)
install(TARGETS slater
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
