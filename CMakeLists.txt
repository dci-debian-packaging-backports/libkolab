project(Libkolab)

cmake_minimum_required(VERSION 2.8.9)

option(BUILD_TESTS "Build the tests" TRUE)
option(PYTHON_BINDINGS "Build bindings for python" FALSE)
option(PHP_BINDINGS "Build bindings for php" FALSE)

set(Libkolab_MODULE_DIR ${Libkolab_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${Libkolab_MODULE_DIR})

# only available from cmake-2.8.0
if(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 7)
    cmake_policy(SET CMP0012 NEW)
endif()

# only available from cmake-2.8.4
if(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} GREATER 7 AND
    ${CMAKE_PATCH_VERSION} GREATER 3)
    cmake_policy(SET CMP0017 NEW)
endif()

# Versioning
# x.y.z scheme
# Development versions are only x.y
#
# i.e.
# 0.1 (0.1 development version towards 0.1.0)
# 0.1.0 (first release)
# 0.1.1 (patch release for 0.1.0)
# 0.2 (0.2 development version towards 0.2.0)
set(Libkolab_VERSION_MAJOR 1)
set(Libkolab_VERSION_MINOR 0)
# Enable the full x.y.z version only for release versions
set(Libkolab_VERSION_PATCH 2)
#set(Libkolab_VERSION ${Libkolab_VERSION_MAJOR}.${Libkolab_VERSION_MINOR}.${Libkolab_VERSION_PATCH})
set(Libkolab_VERSION ${Libkolab_VERSION_MAJOR}.${Libkolab_VERSION_MINOR})
set(Libkolab_VERSION_STRING ${CMAKE_PROJECT_NAME}-${Libkolab_VERSION})

# set up install directories.
set(LIB_INSTALL_DIR lib${LIB_SUFFIX} CACHE STRING "The directories where to install libraries to")
set(INCLUDE_INSTALL_DIR include CACHE STRING "The directory where to install headers to")
set(INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/kolab)
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/Libkolab)

# Make relative paths absolute (needed later on)
foreach(p LIB INCLUDE CMAKECONFIG)
    set(var ${p}_INSTALL_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

include(MacroLogFeature)

find_package(Qt5Core REQUIRED)


# Do the building
find_package(Libkolabxml 1.1.1 REQUIRED)
macro_log_feature(Libkolabxml_FOUND "Libkolabxml" "Kolab XML Format 3 serializing library" "https://git.kolab.org/diffusion/LKX/" TRUE "1.1.1" "Required for reading/writing Kolab XML Objects")

set(QT_REQUIRED_VERSION "5.2.0")

if (Qt5_POSITION_INDEPENDENT_CODE)
    #Requires cmake 2.8.9 (same as -fPIC on gcc)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(KDEPIMLIBS_LIB_VERSION "4.71.0")
set(KMIME_LIB_VERSION "4.84.0")
find_package(KF5CoreAddons CONFIG REQUIRED)
find_package(KF5Akonadi ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarCore ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarUtils ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiNotes ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)

find_package(SWIG)

#Show summary of found libraries
macro_display_feature_log()

# add_definitions(-DKDEPIMLIBS_VERSION=((${KdepimLibs_VERSION_MAJOR}<<16)|(${KdepimLibs_VERSION_MINOR}<<8)|(${KDEPIMLIBS_VERSION_PATCH})))
add_definitions(-DKDEPIMLIBS_VERSION_MAJOR=${KdepimLibs_VERSION_MAJOR})
add_definitions(-DKDEPIMLIBS_VERSION_MINOR=${KdepimLibs_VERSION_MINOR})
add_definitions(-DKDEPIMLIBS_VERSION_PATCH=${KdepimLibs_VERSION_PATCH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-common -Woverloaded-virtual -fno-threadsafe-statics -fvisibility=hidden -Werror=return-type -fvisibility-inlines-hidden -fexceptions -UQT_NO_EXCEPTIONS -fPIC -g -std=c++11")
# message("${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")

set(KDE_INCLUDES ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES})
set(KDE_LIBRARIES
    KF5::CalendarCore
    KF5::CalendarUtils
    KF5::Contacts
    KF5::Mime
    KF5::AkonadiCore
    KF5::AkonadiNotes
)

find_package(Boost REQUIRED)

include_directories(
    ${QT_INCLUDES}
    ${KDE_INCLUDES}
    ${CMAKE_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/kolabformatV2
    ${Libkolabxml_INCLUDES}
    ./
)

configure_file(libkolab-version.h.cmake "${CMAKE_BINARY_DIR}/libkolab-version.h" @ONLY)

add_subdirectory(kolabformatV2)
add_subdirectory(conversion)
add_subdirectory(calendaring)
add_subdirectory(icalendar)
add_subdirectory(freebusy)
add_subdirectory(utils)

QT5_WRAP_CPP(CALENDARING_MOC calendaring/event.h)
# QT5_WRAP_CPP(CONVERSION_MOC conversion/qtevent.h conversion/qtduration.h)

set(KOLAB_SRCS
    kolabformat/kolabobject.cpp
    kolabformat/xmlobject.cpp
    kolabformat/formathelpers.cpp
    kolabformat/errorhandler.cpp
    kolabformat/v2helpers.cpp
	kolabformat/mimeobject.cpp
    mime/mimeutils.cpp
    ${CONVERSION_SRCS}
    ${kolabformatv2_SRCS}
    ${CALENDARING_SRCS}
    ${ICALENDAR_SRCS}
    ${CALENDARING_MOC}
    ${CONVERSION_MOC}
    ${FREEBUSY_SRCS}
)

set(KOLAB_LINK_LIBRARIES
    ${Libkolabxml_LIBRARIES}
    ${KDE_LIBRARIES}
    Qt5::Core
    Qt5::Xml
    Qt5::Gui
    Qt5::Widgets
)

if(BUILD_TESTS)
    find_package(Qt5Test REQUIRED)
    #for tests only
    enable_testing()
    add_library(kolab_static STATIC ${KOLAB_SRCS})
    target_link_libraries(kolab_static ${KOLAB_LINK_LIBRARIES} Qt5::Test)
    add_subdirectory(tests)
endif(BUILD_TESTS)

add_library(kolab SHARED ${KOLAB_SRCS})
target_link_libraries(kolab ${KOLAB_LINK_LIBRARIES})
set_target_properties(kolab PROPERTIES VERSION ${Libkolab_VERSION}
                                     SOVERSION ${Libkolab_VERSION_MAJOR})

install(TARGETS kolab EXPORT LibkolabExport
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

install(FILES
    kolab_export.h
    kolabformat/kolabdefinitions.h
    kolabformat/formathelpers.h
    kolabformat/kolabobject.h
    kolabformat/errorhandler.h
    kolabformat/xmlobject.h
    kolabformat/mimeobject.h
    conversion/kcalconversion.h
    conversion/kabcconversion.h
    conversion/commonconversion.h
    freebusy/freebusy.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)

#Get the include directory relative to CMAKECONFIG_INSTALL_DIR
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKECONFIG_INSTALL_DIR}" "${INCLUDE_INSTALL_DIR}")
#Assemble the full relative path. This will be used in the LibkolabConfig.cmake, which will be installed in CMAKECONFIG_INSTALL_DIR
set(CONF_INCLUDE_DIRS "\${Libkolab_CMAKE_DIR}/${REL_INCLUDE_DIR}")

install(EXPORT LibkolabExport DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE LibkolabTargets.cmake)

configure_file(${Libkolab_MODULE_DIR}/LibkolabConfig.cmake.in ${Libkolab_BINARY_DIR}/LibkolabConfig.cmake @ONLY)
configure_file(${Libkolab_MODULE_DIR}/LibkolabConfigVersion.cmake.in ${Libkolab_BINARY_DIR}/LibkolabConfigVersion.cmake @ONLY)
# Install these two files into the same directory as the generated exports-file.
install(FILES ${Libkolab_BINARY_DIR}/LibkolabConfig.cmake ${Libkolab_BINARY_DIR}/LibkolabConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})


include(SWIGUtils)
if(PYTHON_BINDINGS)
    generatePythonBindings(shared shared.i)
    add_subdirectory(kolabformat/python)
endif(PYTHON_BINDINGS)

if(PHP_BINDINGS)
    generatePHPBindings(kolabshared shared.i)
    generatePHPBindings(dummy dummy.i)
    add_subdirectory(kolabformat/php)
endif(PHP_BINDINGS)
