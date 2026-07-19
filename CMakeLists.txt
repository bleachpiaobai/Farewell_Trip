cmake_minimum_required(VERSION 3.19)
project(Farewell_Trip
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
)

qt_standard_project_setup()

# ── Collect all sources by directory ──────────────────────
file(GLOB_RECURSE CORE_SRC     src/core/*.cpp     src/core/*.h)
file(GLOB_RECURSE UI_SRC       src/ui/*.cpp       src/ui/*.h)
file(GLOB_RECURSE SCENE_SRC    src/scene/*.cpp    src/scene/*.h)
file(GLOB_RECURSE ENTITY_SRC   src/entity/*.cpp   src/entity/*.h)
file(GLOB_RECURSE COMBAT_SRC   src/combat/*.cpp   src/combat/*.h)
file(GLOB_RECURSE ANIM_SRC     src/animation/*.cpp src/animation/*.h)
file(GLOB_RECURSE DIALOGUE_SRC src/dialogue/*.cpp src/dialogue/*.h)
file(GLOB_RECURSE VIDEO_SRC    src/video/*.cpp    src/video/*.h)
file(GLOB_RECURSE AUDIO_SRC    src/audio/*.cpp    src/audio/*.h)
file(GLOB_RECURSE UTILS_SRC    src/utils/*.cpp    src/utils/*.h)
file(GLOB_RECURSE CHAPTER_SRC  src/chapters/*.cpp src/chapters/*.h)

set(RESOURCES resources/resources.qrc)

qt_add_executable(Farewell_Trip
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    ${CORE_SRC}
    ${UI_SRC}
    ${SCENE_SRC}
    ${ENTITY_SRC}
    ${COMBAT_SRC}
    ${ANIM_SRC}
    ${DIALOGUE_SRC}
    ${VIDEO_SRC}
    ${AUDIO_SRC}
    ${UTILS_SRC}
    ${CHAPTER_SRC}
    ${RESOURCES}
)

target_include_directories(Farewell_Trip PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/src/combat
    ${CMAKE_CURRENT_SOURCE_DIR}/src/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogue
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/chapters
)

target_link_libraries(Farewell_Trip PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Multimedia
    Qt::MultimediaWidgets
)

include(GNUInstallDirs)

install(TARGETS Farewell_Trip
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Farewell_Trip
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
