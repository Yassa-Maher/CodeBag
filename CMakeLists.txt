cmake_minimum_required(VERSION 3.19)
project(CS LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(CS
    WIN32 MACOSX_BUNDLE  # ← مطلوب عشان يخلي .exe بدون كونسول ويظهر الأيقونة
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    codeentry.h
    codeentry.cpp
    codedialog.h
    codedialog.cpp
    codedialog.ui
    re.qrc
)

# إضافة ملف .rc للأيقونة (ويندوز فقط)
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
    target_sources(CS PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(CS
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS CS
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CS
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
