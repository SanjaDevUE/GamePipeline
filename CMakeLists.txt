cmake_minimum_required(VERSION 3.24)
project(GamePipeline VERSION 0.4.1 LANGUAGES CXX)

if (WIN32)
    enable_language(RC)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32 AND NOT CMAKE_PREFIX_PATH AND EXISTS "C:/msys64/mingw64/lib/cmake/Qt6/Qt6Config.cmake")
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64")
endif ()

find_package(Qt6 6.5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets)

qt_standard_project_setup()

qt_add_executable(GamePipeline
        MANUAL_FINALIZATION
        src/main.cpp
        src/domain/GameProject.cpp
        src/domain/GameProject.h
        src/domain/ProjectRepository.cpp
        src/domain/ProjectRepository.h
        src/localization/Language.cpp
        src/localization/Language.h
        src/ui/pages/LogsPage.cpp
        src/ui/pages/LogsPage.h
        src/ui/pages/ProjectsPage.cpp
        src/ui/pages/ProjectsPage.h
        src/ui/pages/SteamPage.cpp
        src/ui/pages/SteamPage.h
        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/widgets/LogPanel.cpp
        src/ui/widgets/LogPanel.h
        resources/GamePipeline.qrc)

if (WIN32)
    target_sources(GamePipeline
            PRIVATE
            resources/GamePipeline.rc)
endif ()

target_include_directories(GamePipeline
        PRIVATE
        src)

target_compile_definitions(GamePipeline
        PRIVATE
        GAMEPIPELINE_VERSION="${PROJECT_VERSION}")

target_link_libraries(GamePipeline
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

set_target_properties(GamePipeline PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE)

if (WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    find_program(WINDEPLOYQT_EXECUTABLE
            NAMES windeployqt windeployqt6
            HINTS "${QT_BIN_DIR}")

    if (WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:${PROJECT_NAME}>"
                COMMENT "Deploying Qt runtime dependencies")
    endif ()
endif ()

if (EXISTS "${CMAKE_SOURCE_DIR}/Steam")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/Steam"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Steam"
            COMMENT "Copying portable Steam tools")
endif ()

qt_finalize_executable(GamePipeline)
