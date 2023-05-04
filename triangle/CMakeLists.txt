cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick ShaderTools)

qt_standard_project_setup()

set(SOURCES
    main.cpp
)

qt_add_executable(apptriangle
    ${SOURCES}
)

qt_add_qml_module(apptriangle
    URI triangle
    VERSION 1.0
    QML_FILES Main.qml
)

qt6_add_shaders(apptriangle "shaders"
    BATCHABLE
    PRECOMPILE
    OPTIMIZED
    PREFIX
        "/"
    FILES
        "vertex.vert"
        "fragment.frag"
)

target_link_libraries(apptriangle
    PRIVATE Qt6::Quick
)

install(TARGETS apptriangle
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
