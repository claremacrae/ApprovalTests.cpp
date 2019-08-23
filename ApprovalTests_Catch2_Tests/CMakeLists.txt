project(ApprovalTests_Catch2_Tests)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME}
    main.cpp
    ApprovalExceptionTests.cpp
    ApprovalsTests.cpp
    ApprovalTests.hpp
    CombinationTests.cpp
    FileApproverTests.cpp
    NamerTests.cpp
    PairUtilities.h
    StringUtilsTests.cpp
    StringWriterTests.cpp
    StyleGuide.h
    SystemUtilsTests.cpp
    ToStringExample.cpp
    ToStringTemplateExample.cpp
    ToStringWrapperExample.cpp
    Tutorial.cpp
    VectorTests.cpp
    reporters/ReporterTests.cpp
    reporters/DoNothingLauncher.h
    reporters/FakeReporter.h
    reporters/TestReporter.h
    reporters/ReporterTests.cpp
    utilities/MachineBlockerTests.cpp
    reporters/BlockingReporterTests.cpp
    FileUtilsSystemSpecificTests.cpp
    Catch2DocumentationSamples.cpp
    )
target_link_libraries(${PROJECT_NAME} ApprovalTests)
target_compile_definitions(${PROJECT_NAME} PRIVATE CATCH_CONFIG_FAST_COMPILE)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        )

    if (APPROVALS_SANITIZE_ADDRESS)
        message("APPROVALS_SANITIZE_ADDRESS is enabled")
        # https://stackoverflow.com/questions/44320465/whats-the-proper-way-to-enable-addresssanitizer-in-cmake-that-works-in-xcode
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
        message("${CMAKE_CXX_FLAGS_DEBUG} = " ${CMAKE_CXX_FLAGS_DEBUG})
        message("${CMAKE_LINKER_FLAGS_DEBUG} = " ${CMAKE_LINKER_FLAGS_DEBUG})
    endif ()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /WX
        )
endif()
