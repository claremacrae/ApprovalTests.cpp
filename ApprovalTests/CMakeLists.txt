project(ApprovalTests CXX)
set(CMAKE_CXX_STANDARD 11)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
if (APPROVALS_SANITIZE_ADDRESS)
    message("APPROVALS_SANITIZE_ADDRESS is enabled")

    #  target_compile_options may only be set INTERFACE properties on INTERFACE targets
    target_compile_options(${PROJECT_NAME} INTERFACE
        "-fno-omit-frame-pointer -fsanitize=address")

    #   Unknown CMake command "target_link_options".
    #    target_link_options( ${PROJECT_NAME} PUBLIC
    #        "-fno-omit-frame-pointer -fsanitize=address")

    set_target_properties( ${PROJECT_NAME} PROPERTIES LINK_FLAGS
        "-fno-omit-frame-pointer -fsanitize=address")
endif()
