cmake_minimum_required(VERSION 3.14)

project(NatsGateWay LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
add_subdirectory(QtNats)
add_subdirectory(QtService)

add_executable(NatsGateWay
  main.cpp

  src/singletones/settings.h src/singletones/settings.cpp
  src/apihandler.h src/apihandler.cpp
  src/api/natshandler.h src/api/natshandler.cpp
  src/api/tcpclient.h src/api/tcpclient.cpp
  src/natsservice.h src/natsservice.cpp
)
target_link_libraries(NatsGateWay
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    qtnats
    Qt6Service
)

target_compile_definitions(Qt6Service PUBLIC QT_STATIC=1)

target_include_directories(NatsGateWay PUBLIC
    src/
    QtNats/src/
)

include(GNUInstallDirs)
install(TARGETS NatsGateWay
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(CONFSRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/configs)
set(CONFDESTINDIR ${CMAKE_CURRENT_BINARY_DIR}/configs)

if (UNIX AND NOT APPLE)
    set_target_properties(qstandard qsystemd
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
    )
    configure_file(./buildScriptLinux ${CMAKE_CURRENT_BINARY_DIR}/buildScriptLinux COPYONLY)
    configure_file(./NatsGateWay.conf ${CMAKE_CURRENT_BINARY_DIR}/NatsGateWay.conf COPYONLY)
    configure_file(./NatsGateWay.service ${CMAKE_CURRENT_BINARY_DIR}/NatsGateWay.service COPYONLY)
    file(GLOB templateFiles RELATIVE ${CONFSRCDIR} "${CONFSRCDIR}/*")
    foreach(templateFile ${templateFiles})
        configure_file(${CONFSRCDIR}/${templateFile} ${CONFDESTINDIR}/${templateFile} COPYONLY)
    endforeach(templateFile)
    message(STATUS "copied files!")
endif()

if (WIN32)

    set_target_properties(qstandard qwindows
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/QtService/plugins/servicebackends"
    )
endif()
