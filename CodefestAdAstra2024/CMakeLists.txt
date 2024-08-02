cmake_minimum_required(VERSION 3.10)
project(CodefestAdAstra2024)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Satellite.cpp
    src/GroundStation.cpp
    src/cipher.cc
)

# Add executable
add_executable(CodefestAdAstra2024 ${SOURCES})

# Link libraries
target_link_libraries(CodefestAdAstra2024 ${OPENSSL_LIBRARIES} ${OpenCV_LIBS})
