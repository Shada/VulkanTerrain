file(
    GLOB
    MY_MATH_SOURCES
    ./*
)

add_library(MathyMath SHARED ${MY_MATH_SOURCES})

install (TARGETS MathyMath DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)