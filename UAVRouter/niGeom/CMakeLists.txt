PROJECT(niGeom)

INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
)

FILE(GLOB niGeom_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${niGeom_SRCS})

INSTALL(
        TARGETS ${PROJECT_NAME}
        DESTINATION lib)

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/niGeom/*.h")

INSTALL(
        DIRECTORY ./niGeom
        DESTINATION include)

link_directories(${CMAKE_CURRENT_BINARY_DIR})
