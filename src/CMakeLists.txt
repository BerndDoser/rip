include_directories(
    ${LAME_INCLUDE_DIR}
)

add_executable(
    rip
    rip.cpp
)
    
target_link_libraries(
    rip
    ${LAME_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS rip RUNTIME DESTINATION bin)
