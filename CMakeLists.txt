cmake_minimum_required(VERSION 3.19)
project(Farewell_Trip
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Resources are loaded from disk via CMake copy + GameConfig::imagePath().
# Only tiny assets (styles + icons, ~9 KB) are embedded in the .qrc.
# All images (~181 MB) and videos are copied to the build directory.

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    OpenGLWidgets
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

# ── Copy video files to build output ──
file(GLOB_RECURSE VIDEO_FILES
    resources/videos/*.mp4
)
foreach(VIDEO_FILE ${VIDEO_FILES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/resources ${VIDEO_FILE})
    configure_file(${VIDEO_FILE} ${CMAKE_BINARY_DIR}/${REL_PATH} COPYONLY)
endforeach()

# ── Copy image files to build output (~181 MB, too large to embed in .qrc) ──
# Directory names match Farewell_Trip.doc and code :/images/... paths exactly.
file(COPY resources/images/ch01_awaken/   DESTINATION ${CMAKE_BINARY_DIR}/images/ch01_awaken)
file(COPY resources/images/ch02_peach/    DESTINATION ${CMAKE_BINARY_DIR}/images/ch02_peach)
file(COPY resources/images/ch03_rebellion/ DESTINATION ${CMAKE_BINARY_DIR}/images/ch03_rebellion)
file(COPY resources/images/ch04_survivor/ DESTINATION ${CMAKE_BINARY_DIR}/images/ch04_survivor)
file(COPY resources/images/ch05_farewell/ DESTINATION ${CMAKE_BINARY_DIR}/images/ch05_farewell)
file(COPY resources/images/YAN_Action/    DESTINATION ${CMAKE_BINARY_DIR}/images/YAN_Action)
file(COPY resources/images/Peach_Action/  DESTINATION ${CMAKE_BINARY_DIR}/images/Peach_Action)
file(COPY resources/images/XIA_Action/    DESTINATION ${CMAKE_BINARY_DIR}/images/XIA_Action)
file(COPY resources/images/Doc_Action/    DESTINATION ${CMAKE_BINARY_DIR}/images/Doc_Action)
file(COPY resources/images/Map/           DESTINATION ${CMAKE_BINARY_DIR}/images/Map)
# Single files
file(COPY resources/images/menu.png       DESTINATION ${CMAKE_BINARY_DIR}/images)
file(COPY resources/images/fail.png       DESTINATION ${CMAKE_BINARY_DIR}/images)

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
    Qt::OpenGLWidgets
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
