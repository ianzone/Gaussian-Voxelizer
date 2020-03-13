cmake_minimum_required(VERSION 2.4)

project(Gaussian_Voxelizer)

configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h")

option(DBG "print dbg msg" OFF)

aux_source_directory(. SRCS)

add_executable(gv ${SRCS})

target_link_libraries(gv gmpxx gmp)