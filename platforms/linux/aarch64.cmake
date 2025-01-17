MESSAGE(STATUS "Configure Cross Compiler")

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR aarch64)

SET(TARGET_OS ${CMAKE_SYSTEM_NAME})
SET(TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})

# set CMAKE_C_FLAGS and CMAKE_CXX_FLAGS flag for cross-compiled process
SET(CROSS_COMPILATION_ARM Android)
SET(CROSS_COMPILATION_ARCHITECTURE aarch64)

SET(CMAKE_C_COMPILER       ${TOOLCHAIN_ROOTDIR}/clang)
SET(CMAKE_CXX_COMPILER     ${TOOLCHAIN_ROOTDIR}/clang++)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -march=armv8-a -mcpu=cortex-a53 -Wl,--allow-shlib-undefined")
SET(CMAKE_CXX_FLAGS_DEBUG " -Wall -Werror -g -O0 ")
SET(CMAKE_C_FLAGS_DEBUG " -Wall -Werror -g -O0 ")
SET(CMAKE_CXX_FLAGS_RELEASE " -Wall -Werror -O3 ")
SET(CMAKE_C_FLAGS_RELEASE " -Wall -Werror -O3 ")

# set searching rules for cross-compiler
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# SET(CMAKE_SKIP_BUILD_RPATH TRUE)
# SET(CMAKE_SKIP_RPATH TRUE)

# set g++ param
# -fopenmp link libgomp
# SET(CMAKE_CXX_FLAGS "-std=c++17 -march=armv8-a -mfloat-abi=softfp -mfpu=neon-vfpv4 \
#     -ffunction-sections \
#     -fdata-sections -O2 -fstack-protector-strong -lm -ldl -lstdc++\
#     ${CMAKE_CXX_FLAGS}")