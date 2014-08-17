cmake_minimum_required(VERSION 2.8.12)

project(IviasClient)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "-std=c++11")

    
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/qml)

set(QML_IMPORT_PATH ${QT_IMPORTS_DIR})
set(QML2_IMPORT_PATH ${QML_IMPORT_PATH})

add_definitions(-DQML_IMPORT_PATH=${QML_IMPORT_PATH})
add_definitions(-DQML2_IMPORT_PATH=${QML2_IMPORT_PATH})


# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
#find_package(Qt5Declarative REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Sql REQUIRED)


add_subdirectory( upowerbackend )

# Scan qml directory
file( GLOB_RECURSE QmlPrj_Files ./qml/* )

set( IviasClient_SRCS main.cpp
     adsmodel.cpp 
     helper.cpp 
     clickscounter.cpp 
     package.cpp
     powermanager.cpp 
     updater.cpp
     advertisementslist.cpp
     qmlsettingssingleton.cpp
     inputactivityfilter.cpp
     dummypowerbackend.cpp
     powerbackendiface.h
     ${QmlPrj_Files} )

# Tell CMake to create the IviasClient executable
add_executable(IviasClient ${IviasClient_SRCS})

target_link_libraries(IviasClient Qt5::Core Qt5::Quick Qt5::Sql)

install( TARGETS IviasClient DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
install( FILES ${QmlPrj_Files} DESTINATION ${CMAKE_QML_OUTPUT_DIRECTORY} )
