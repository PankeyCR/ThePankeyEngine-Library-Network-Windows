cmake_minimum_required(VERSION 3.15)

project(ThePankeyEngine_Library_Network_Windows)

if(NOT TARGET ThePankeyEngine_Library_Network_Windows)

    add_library(ThePankeyEngine_Library_Network_Windows INTERFACE)

    target_include_directories(
        ThePankeyEngine_Library_Network_Windows
          INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

endif()