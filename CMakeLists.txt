cmake_minimum_required(VERSION 3.10)
project(F2MPx)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force 32-bit build
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM Win32)
    add_compile_options(/arch:IA32)
    
    # Add MSVC-specific flags
    add_compile_options(/W4 /WX- /MP)
    
    # Disable specific warnings
    add_compile_options(/wd4100) # unreferenced formal parameter
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
endif()

# Collect all source files recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "*.cpp"
)

# Create the executable
add_executable(F2MPx WIN32 ${SOURCES})

# Set output directories
set_target_properties(F2MPx PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add include directories
target_include_directories(F2MPx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(F2MPx PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Link against Windows libraries
target_link_libraries(F2MPx PRIVATE
    kernel32
    user32
) 