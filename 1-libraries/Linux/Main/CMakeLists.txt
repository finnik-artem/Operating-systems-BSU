add_executable(Main main.cpp)

target_link_libraries(Main PRIVATE NumberLib VectorLib)

target_include_directories(Main PRIVATE
    ${CMAKE_SOURCE_DIR}/NumberLib
    ${CMAKE_SOURCE_DIR}/VectorLib
)