#
#--- Configures compiler for C++11
#
if(UNIX)
  if(NOT APPLE)
    # Enable c++11 for GCC
    message(STATUS "Using GCC Compiler.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(GCC_VERSION_MIN "5.0.0.0")
    execute_process(COMMAND echo -n "Checking GCC version --> ")
    execute_process(COMMAND gcc -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(GCC_VERSION VERSION_LESS 5.0.0.0)
      message(STATUS "GCC 4+ version is using")
    endif()
  else()
    # Clang in OSX supports partially c++11 through extensions
    message(STATUS "Using CLANG Intel Compiler. (Untested)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions")
  endif()
else()
  # MSVC12 supports c++11 natively
  message(STATUS "Using MSVC Intel Compiler.")
endif()
