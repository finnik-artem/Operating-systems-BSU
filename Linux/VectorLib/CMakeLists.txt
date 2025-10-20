add_library(VectorLib SHARED
        Vector.cpp
        Vector.h
)

target_link_libraries(VectorLib PUBLIC NumberLib)

target_include_directories(VectorLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/NumberLib
)

target_compile_features(VectorLib PRIVATE cxx_std_20)