cmake_minimum_required(VERSION 3.10)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")  
set(PROJECT_VESRION 0.0.${PATCH_VERSION})  
project(async VERSION ${PROJECT_VESRION})



add_executable(async main.cpp)
add_library(libasync async.cpp)

set_target_properties(async PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
set_target_properties(libasync PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(libasync
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(async PRIVATE libasync)



install(TARGETS async RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

