# Vega - Copyright (C) 2012-2013 Alneos
#
# See the LICENSE.txt file for license information. Please report all
# bugs and problems to <info@alneos.fr>.
# Initial version from cmake config in : http://geuz.org/gmsh

cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)

# Compatibily option with Cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

########################################################################
# Provide add_compile_options() when not available
########################################################################
if(CMAKE_VERSION VERSION_LESS "2.8.12")
    function(add_compile_options)
        add_definitions(${ARGN})
    endfunction(add_compile_options)
endif()

# Definition of the VEGA version
set(VEGA_MAJOR_VERSION 3)
set(VEGA_MINOR_VERSION 1)
set(VEGA_PATCH_VERSION 0)
IF(NOT DEFINED VEGA_EXTRA_VERSION OR "${VEGA_EXTRA_VERSION}" STREQUAL "")
    IF (NOT "$ENV{CI_COMMIT_SHA}" STREQUAL "")
        message(STATUS "CI_COMMIT_SHA:" $ENV{CI_COMMIT_SHA})
        string(SUBSTRING $ENV{CI_COMMIT_SHA} 0 7 VEGA_EXTRA_VERSION)
    ELSE()
        set(VEGA_EXTRA_VERSION "" CACHE STRING "Vega extra version string")
    ENDIF()
ELSE()
    message(STATUS "VEGA_EXTRA_VERSION: " ${VEGA_EXTRA_VERSION})
ENDIF()

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/cmake_files)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_BUILD_TYPE DDebug CACHE STRING
    "Choose the type of build, options are: None DDebug SDebug DRelease SRelease.")
#make outputs build commands and options
OPTION(CMAKE_VERBOSE_MAKEFILE "make outputs build commands" OFF)
project ( vegapp CXX C)

if(CMAKE_CROSSCOMPILING)
  find_program(WINE_EXECUTABLE NAMES wine64 wine)
  set(CMAKE_CROSSCOMPILING_EMULATOR ${WINE_EXECUTABLE})
endif(CMAKE_CROSSCOMPILING)

find_program(SYSTUS_EXEC NAMES systus systus.exe)
if(SYSTUS_EXEC)
    set(RUN_SYSTUS ON CACHE BOOL "Run tests using Systus executable")
    message(STATUS "Sytus exec: " ${SYSTUS_EXEC})
else(SYSTUS_EXEC)
    set(RUN_SYSTUS OFF CACHE BOOL "Run tests using Systus executable")
endif(SYSTUS_EXEC)

mark_as_advanced(RUN_SYSTUS)

find_program(ASTER_EXEC NAMES as_run as_run.sh as_run.exe as_run.bat)
if(ASTER_EXEC)
    set(RUN_ASTER ON CACHE BOOL "Run tests using Aster executable")
    message(STATUS "Aster exec: " ${ASTER_EXEC})
else(ASTER_EXEC)
    set(RUN_ASTER OFF CACHE BOOL "Run tests using Aster executable")
endif(ASTER_EXEC)
mark_as_advanced(RUN_ASTER)

find_program(NASTRAN_EXEC NAMES nastran)
if(NASTRAN_EXEC)
    set(RUN_NASTRAN ON CACHE BOOL "Run tests using Nastran executable")
    message(STATUS "Nastran exec: " ${NASTRAN_EXEC})
else(NASTRAN_EXEC)
    set(RUN_NASTRAN OFF CACHE BOOL "Run tests using Nastran executable")
endif(NASTRAN_EXEC)
mark_as_advanced(RUN_NASTRAN)

# Debug and Release are reserved words for cmake see https://cmake.org/cmake/help/v3.0/variable/CMAKE_BUILD_TYPE.html
IF(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
    message(STATUS "Debug mode selected")
    option(STATIC_LINKING "Static Linking" OFF)
    option(ENABLE_DEBUG "Debug" ON)
    option(ENABLE_CODECOVERAGE "Code Coverage" ON)
ENDIF(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
IF(CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
    message(STATUS "Dynamic Release mode selected")
    option(STATIC_LINKING "Static Linking" OFF)
    option(ENABLE_DEBUG "Debug" OFF)
    option(ENABLE_CODECOVERAGE "Code Coverage" OFF)
ENDIF(CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")

# Other legacy configurations (used by ESI)
IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Dd][Ee][Bb][Uu][Gg]")
    message(STATUS "Dynamic Debug mode selected")
    option(STATIC_LINKING "Static Linking" OFF)
    option(ENABLE_DEBUG "Debug" ON)
    option(ENABLE_CODECOVERAGE "Code Coverage" ON)
    add_definitions(-DVDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES "[Dd][Dd][Ee][Bb][Uu][Gg]")
IF(CMAKE_BUILD_TYPE MATCHES "[Ss][Dd][Ee][Bb][Uu][Gg]")
    message(STATUS "Static Debug mode selected")
    option(STATIC_LINKING "Static Linking" ON)
    option(ENABLE_DEBUG "Debug" ON)
    option(ENABLE_CODECOVERAGE "Code Coverage" ON)
    add_definitions(-DVDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES "[Ss][Dd][Ee][Bb][Uu][Gg]")
IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
    message(STATUS "Dynamic Release mode selected")
    option(STATIC_LINKING "Static Linking" OFF)
    option(ENABLE_DEBUG "Debug" OFF)
    option(ENABLE_CODECOVERAGE "Code Coverage" OFF)
ENDIF(CMAKE_BUILD_TYPE MATCHES "[Dd][Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
IF(CMAKE_BUILD_TYPE MATCHES "[Ss][Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
    message(STATUS "Static Release mode selected")
    option(STATIC_LINKING "Static Linking" ON)
    option(ENABLE_DEBUG "Debug" OFF)
    option(ENABLE_CODECOVERAGE "Code Coverage" OFF)
ENDIF(CMAKE_BUILD_TYPE MATCHES "[Ss][Rr][Ee][Ll][Ee][Aa][Ss][Ee]")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules/")

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-Wshadow=local COMPILER_SUPPORTS_WSHADOWLOCAL)
if(COMPILER_SUPPORTS_WSHADOWLOCAL)
  add_compile_options(-Wshadow=local)
endif()

CHECK_CXX_COMPILER_FLAG(-fext-numeric-literals COMPILER_SUPPORTS_FEXTNUMERICLITERALS)
if(COMPILER_SUPPORTS_FEXTNUMERICLITERALS)
  add_compile_options(-fext-numeric-literals)
endif()

CHECK_CXX_COMPILER_FLAG(-Wduplicated-cond COMPILER_SUPPORTS_DUPLICATED_COND)
if(COMPILER_SUPPORTS_DUPLICATED_COND)
  add_compile_options(-Wduplicated-cond)
endif()

CHECK_CXX_COMPILER_FLAG(-Wfloat-equal COMPILER_SUPPORTS_FLOAT_EQUAL)
if(COMPILER_SUPPORTS_FLOAT_EQUAL)
  add_compile_options(-Wfloat-equal)
endif()

CHECK_CXX_COMPILER_FLAG(-Wsuggest-override COMPILER_SUPPORTS_SUGGEST_OVERRIDE)
if(COMPILER_SUPPORTS_SUGGEST_OVERRIDE)
  add_compile_options(-Wsuggest-override)
endif()

CHECK_CXX_COMPILER_FLAG(-Wsuggest-final-methods COMPILER_SUPPORTS_SUGGEST_FINAL_METHODS)
if(COMPILER_SUPPORTS_SUGGEST_FINAL_METHODS)
  add_compile_options(-Wsuggest-final-methods)
endif()

CHECK_CXX_COMPILER_FLAG(-Wsuggest-final-types COMPILER_SUPPORTS_SUGGEST_FINAL_TYPES)
if(COMPILER_SUPPORTS_SUGGEST_FINAL_TYPES)
  add_compile_options(-Wsuggest-final-types)
endif()

CHECK_CXX_COMPILER_FLAG(-Wnon-virtual-dtor COMPILER_SUPPORTS_NON_VIRTUAL_DTOR)
if(COMPILER_SUPPORTS_NON_VIRTUAL_DTOR)
  add_compile_options(-Wnon-virtual-dtor)
endif()

CHECK_CXX_COMPILER_FLAG(-Woverloaded-virtual COMPILER_SUPPORTS_OVERLOADED_VIRTUAL)
if(COMPILER_SUPPORTS_OVERLOADED_VIRTUAL)
  add_compile_options(-Woverloaded-virtual)
endif()

#CHECK_CXX_COMPILER_FLAG(-fsanitize=null COMPILER_SUPPORTS_SANITIZE_NULL)
#if(COMPILER_SUPPORTS_SANITIZE_NULL)
#  add_compile_options(-fsanitize=null)
#endif()

# boost variant constructor uses this see ValueOrReference
#CHECK_CXX_COMPILER_FLAG(-Wzero-as-null-pointer-constant COMPILER_SUPPORTS_ZERO_AS_NULL_POINTER_CONSTANT)
#if(COMPILER_SUPPORTS_ZERO_AS_NULL_POINTER_CONSTANT)
#  add_compile_options(-Wzero-as-null-pointer-constant)
#endif()

# boost::qi seems to prefer to initialize things inside constructor, see CSVResultReader
#CHECK_CXX_COMPILER_FLAG(-Weffc++ COMPILER_SUPPORTS_EFF_CPP)
#if(COMPILER_SUPPORTS_EFF_CPP)
#  add_compile_options(-Weffc++)
#endif()

if(MSVC)
    # Patch for MSVC10 generator, 64 bit
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -DBOOST_ALL_NO_LIB)
    if(STATIC_LINKING)
        #for now under windows don't use static runtime
        #add_compile_options(/MT)
        #set(CMAKE_EXE_LINKER_FLAGS "-pthread -static-libgcc -static-libstdc++ -static" CACHE INTERNAL "Linker flags")
    else(STATIC_LINKING)
        ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
        #set(CMAKE_EXE_LINKER_FLAGS "-pthread -shared-libgcc -rdynamic -pipe" CACHE INTERNAL "Linker flags")
    endif(STATIC_LINKING)
else(MSVC)

    if(EXISTS "/etc/issue")
        file(READ "/etc/issue" ETC_ISSUE)
        string(REGEX MATCH "Debian|Ubuntu|Alpine" LINUX_DIST ${ETC_ISSUE})
    endif()

    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstack-protector-all")
    #SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    #set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -Wl,--strip-all")
    add_compile_options(-std=c++11 -Werror -Wall -Wextra -Winit-self -Wconversion -pedantic -pedantic-errors)
    IF(CMAKE_C_COMPILER_ID MATCHES "Intel")
        add_compile_options(-diag-disable 1170 -fmessage-length=0) # -Wshadow
    ELSEIF(CMAKE_C_COMPILER_ID MATCHES "GNU")
        add_compile_options(-Woverlength-strings -fmessage-length=0 -Wmain -Wmissing-declarations -Wundef -Wredundant-decls -Wcast-align -Wswitch-default -Wlogical-op -Wuseless-cast -Wold-style-cast -Wdouble-promotion -Wunused-variable)

        # try to reduce boost memory usage for Travis but causes out of time, it is better to use make -j1
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --param ggc-min-expand=1 --param ggc-min-heapsize=100000")

        if(NOT CMAKE_CROSSCOMPILING)
            add_compile_options(-Wsuggest-attribute=const) # LD strange results, probably because MXE is using 5.4.0
        endif()
        if(ENABLE_CODECOVERAGE)
          find_program( GCOV_PATH gcov )
        else(ENABLE_CODECOVERAGE)
          if(ENABLE_DEBUG)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
          endif(ENABLE_DEBUG)
        endif(ENABLE_CODECOVERAGE)
        if(ENABLE_CODECOVERAGE AND GCOV_PATH)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
            list(APPEND EXTERNAL_LIBRARIES gcov)
            set(CTEST_COVERAGE_COMMAND "gcov")
        endif(ENABLE_CODECOVERAGE AND GCOV_PATH)
    ENDIF()
    if(STATIC_LINKING)
        if(NOT CMAKE_CROSSCOMPILING)
            add_compile_options(-fPIE)
        endif(NOT CMAKE_CROSSCOMPILING)
        if(NOT LINUX_DIST STREQUAL "Ubuntu")
          set(CMAKE_EXE_LINKER_FLAGS_EXTRA "-static") # cannot statically link on ubuntu because of glibc, so I do it only on Alpine
        endif()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -static-libgcc -static-libstdc++ ${CMAKE_EXE_LINKER_FLAGS_EXTRA}" CACHE INTERNAL "Linker flags")
    else(STATIC_LINKING)
        if(NOT CMAKE_CROSSCOMPILING)
        	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -shared-libgcc -pipe")
    	endif(NOT CMAKE_CROSSCOMPILING)
    endif(STATIC_LINKING)

    IF(CYGWIN)
        # Specific options for Cygwin
        add_compile_options(-D_XOPEN_SOURCE=500)
    ELSE(CYGWIN)
        # Specific options for Linux compilations
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        if(NOT STATIC_LINKING)
            if(NOT CMAKE_CROSSCOMPILING)
                set(CMAKE_POSITION_INDEPENDENT_CODE ON)
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic") # LD: I think is used only by glib backtrace ?
            endif(NOT CMAKE_CROSSCOMPILING)
        endif(NOT STATIC_LINKING)
    ENDIF(CYGWIN)

    # Optional use of ccache
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
       SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
       SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)

endif(MSVC)

#set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)
# this variable controls the default value of the "ENABLE_XXX" options which are
# normally set to ON (useful if you want to configure a minimal version of Vega:
# e.g. "cmake -DDEFAULT=0 -DENABLE_NASTRAN=1 -DENABLE_SYSTUS=1")
set(DEFAULT ON CACHE INTERNAL "Default value for enabled-by-default options")

option(ENABLE_ASTER "Enable Aster" ${DEFAULT})
IF(DEFINED ENABLE_ASTER)
	option(ENABLE_ASTER "Enable Aster" ${ENABLE_ASTER})
ENDIF(DEFINED ENABLE_ASTER)
option(ENABLE_NASTRAN "Enable Nastran" ${DEFAULT})
option(ENABLE_SYSTUS "Enable Systus" ${DEFAULT})
option(ENABLE_WRAP_PYTHON "Build Python wrappers" OFF)

set(VEGA_VERSION "${VEGA_MAJOR_VERSION}.${VEGA_MINOR_VERSION}")
set(VEGA_VERSION "${VEGA_VERSION}.${VEGA_PATCH_VERSION} ${VEGA_EXTRA_VERSION}")
set(VEGA_SHORT_LICENSE "GPL")

execute_process(COMMAND date "+%Y%m%d" OUTPUT_VARIABLE DATE
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND hostname OUTPUT_VARIABLE HOSTNAME
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND whoami OUTPUT_VARIABLE PACKAGER
                OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT DATE)
  set(DATE "unknown")
endif(NOT DATE)
set(VEGA_DATE "${DATE}")

if(NOT HOSTNAME)
  set(HOSTNAME "unknown")
endif(NOT HOSTNAME)
set(VEGA_HOST "${HOSTNAME}")

if(NOT PACKAGER)
  set(PACKAGER "unknown")
endif(NOT PACKAGER)
set(VEGA_PACKAGER "${PACKAGER}")

if(APPLE)
  set(VEGA_OS "MacOSX")
else(APPLE)
  set(VEGA_OS "${CMAKE_SYSTEM_NAME}")
endif(APPLE)

macro(set_config_option VARNAME STRING)
  set(${VARNAME} TRUE)
  list(APPEND CONFIG_OPTIONS ${STRING})
  message(STATUS "Found " ${STRING})
endmacro(set_config_option)

# Before the add_subdirectory command(s)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET (CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#10 hours for tests
set (CTEST_TEST_TIMEOUT 36000 CACHE STRING
    "Maximum time allowed before CTest will kill the test.")
set (CTEST_OUTPUT_ON_FAILURE ON)

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCXXCompilerFlag)

# check if the machine is 64 bits (this is more reliable than using
# CMAKE_SIZEOF_VOID_P, which does not seem to work e.g. on some Suse
# machines)
#check_type_size("void*" SIZEOF_VOID_P) not working at the moments
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set_config_option(HAVE_64BIT_SIZE_T "Have64BitSizeT")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

set(Boost_USE_STATIC_LIBS ${STATIC_LINKING})
#set(Boost_USE_MULTITHREADED ON)
IF(NOT WIN32)
    #under windows don't use static runtime because medC hasn't been compiled with
    #(see commented compile option /MT)
    set(Boost_USE_STATIC_RUNTIME ${STATIC_LINKING})
ENDIF(NOT WIN32)

find_package(Boost 1.54.0 COMPONENTS thread date_time program_options filesystem system regex unit_test_framework REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories ( ${Boost_LIBRARY_DIRS} )
list(APPEND EXTERNAL_LIBRARIES ${Boost_LIBRARIES})

# Bug fix for https://stackoverflow.com/questions/35007134/c-boost-undefined-reference-to-boostfilesystemdetailcopy-file
IF("105800" STRGREATER ${Boost_VERSION})
  message(STATUS "BOOST VERSION UNDER 1.58, BOOST_NO_CXX11_SCOPED_ENUMS is defined.")
  add_definitions(-DBOOST_NO_CXX11_SCOPED_ENUMS)
ENDIF("105800" STRGREATER ${Boost_VERSION})

# On Cygwin, we need to explicitely look some libraries on /lib
IF(CYGWIN)
  link_directories("${CYGWIN_MOUNT}/lib")
ENDIF(CYGWIN)

IF (ENABLE_CODECOVERAGE)
  if (LINUX_DIST STREQUAL "Alpine")
    set(Backtrace_FOUND OFF)
  else()
    find_package(Backtrace)
  endif()

  IF(Backtrace_FOUND)
    list(APPEND EXTERNAL_LIBRARIES ${Backtrace_LIBRARIES})
    include_directories(SYSTEM ${Backtrace_INCLUDE_DIRS})
  ELSE()
    set(LIBUNWIND_USE_STATIC_LIBS ${STATIC_LINKING})
    find_package(Libunwind)
    IF(LIBUNWIND_FOUND)
        include_directories(SYSTEM ${LIBUNWIND_INCLUDE_DIR})
        list(APPEND EXTERNAL_LIBRARIES ${LIBUNWIND_LIBRARIES})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-allow-multiple-definition" CACHE INTERNAL "Linker flags")
    ENDIF()
  ENDIF()
ENDIF()

find_package(Valgrind)

#necessary to find build_properties.h
include_directories(${CMAKE_BINARY_DIR})

if(ENABLE_ASTER)
  set_config_option(HAVE_ASTER "Aster")
  add_subdirectory(Aster)
endif(ENABLE_ASTER)

if(ENABLE_NASTRAN)
  set_config_option(HAVE_NASTRAN "Nastran")
  add_subdirectory(Nastran)
  add_subdirectory(Optistruct)
endif(ENABLE_NASTRAN)

if(ENABLE_SYSTUS)
  set_config_option(HAVE_SYSTUS "Systus")
  add_subdirectory(Systus)
endif(ENABLE_SYSTUS)

add_subdirectory(Abstract)
add_subdirectory(ResultReaders)
add_subdirectory(Commandline)

INCLUDE(CTest)

IF(NIGHTLY_BUILD MATCHES "TRUE")
   set_config_option(HAVE_LONG_TESTS "LongTests")
ENDIF(NIGHTLY_BUILD MATCHES "TRUE")

IF(BUILD_TESTING)
    SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
    MARK_AS_ADVANCED(BUILDNAME)
    # Tentative fix for issue in Jenkins docker (picking dynamic library even when static is demanded and present
    # See https://stackoverflow.com/questions/14077611/how-do-i-tell-cmake-to-link-in-a-static-library-in-the-source-directory
    if(STATIC_LINKING)
        ADD_LIBRARY(boost_unit_test_framework STATIC IMPORTED)
    else(STATIC_LINKING)
        ADD_LIBRARY(boost_unit_test_framework SHARED IMPORTED)
    endif(STATIC_LINKING)
    SET_TARGET_PROPERTIES(boost_unit_test_framework PROPERTIES IMPORTED_LOCATION ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    ENABLE_TESTING()
    add_subdirectory(Test)
ENDIF()

list(SORT CONFIG_OPTIONS)
set(VEGA_CONFIG_OPTIONS "")
foreach(OPT ${CONFIG_OPTIONS})
  set(VEGA_CONFIG_OPTIONS "${VEGA_CONFIG_OPTIONS} ${OPT}")
endforeach(OPT)


if(ENABLE_WRAP_PYTHON)
  find_package(SWIG)
  find_package(PythonLibs)
  if(SWIG_FOUND AND PYTHONLIBS_FOUND)
    message(STATUS "Found SWIG version " ${SWIG_VERSION})
    string(SUBSTRING ${SWIG_VERSION} 0 1 SWIG_MAJOR_VERSION)
    if(SWIG_MAJOR_VERSION EQUAL 1)
      message("WARNING: Python bindings require SWIG >= 2: disabling Python")
    else(SWIG_MAJOR_VERSION EQUAL 1)
      add_subdirectory(wrappers/vegapy)
    endif(SWIG_MAJOR_VERSION EQUAL 1)
  endif(SWIG_FOUND AND PYTHONLIBS_FOUND)
endif(ENABLE_WRAP_PYTHON)

mark_as_advanced(VEGA_EXTRA_VERSION Boost_DIR)

##build Doxygen documentation
find_package(Doxygen)

if(DEFINED DOXYGEN_BUILD)
    if(NOT DOXYGEN_BUILD)
        message(STATUS "Doxygen has been disabled by the user.")
        set(DOXYGEN_FOUND OFF)
    endif(NOT DOXYGEN_BUILD)
endif(DEFINED DOXYGEN_BUILD)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)


message("")
message(STATUS "Vega ${VEGA_VERSION} has been configured for ${VEGA_OS} with the "  "following options:" ${VEGA_CONFIG_OPTIONS})
message(STATUS "CMAKE_VERSION:        ${CMAKE_VERSION}")
message(STATUS "C compiler: " ${CMAKE_C_COMPILER})
message(STATUS "C++ compiler: " ${CMAKE_CXX_COMPILER})
message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
message(STATUS "CMAKE_CROSSCOMPILING_EMULATOR: ${CMAKE_CROSSCOMPILING_EMULATOR}")
message(STATUS "CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message(STATUS "CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")
message(STATUS "LINUX_DIST:           ${LINUX_DIST}")

get_property(INC_DIR DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message(STATUS "Include directories: ${INC_DIR}" )
message(STATUS "External libraries: ${EXTERNAL_LIBRARIES}" )
message(STATUS "C compiler flags: " ${CMAKE_C_FLAGS})
message(STATUS "C++ compiler flags: " ${CMAKE_CXX_FLAGS})
message(STATUS "C++ linker flags: " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

message(STATUS "Install prefix: " ${CMAKE_INSTALL_PREFIX})
if(DOXYGEN_FOUND)
  message(STATUS "Doxygen: " ${DOXYGEN_EXECUTABLE})
  message(STATUS "Doxyfile: " ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
else(DOXYGEN_FOUND)
  message(STATUS "Doxygen: not found or disabled.")
endif(DOXYGEN_FOUND)
if(VALGRIND_FOUND)
  message(STATUS "Valgrind: " ${Valgrind_EXECUTABLE})
  message(STATUS "Valgrind include dir: " ${Valgrind_INCLUDE_DIR})
else(VALGRIND_FOUND)
  message(STATUS "Valgrind: not found or disabled.")
endif(VALGRIND_FOUND)
message(STATUS "Boost unit test library: " ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
if(${STATIC_LINKING})
    message(STATUS "Static linking.")
else(${STATIC_LINKING})
    message(STATUS "Dynamic linking.")
endif(${STATIC_LINKING})
#message("")
#message("Run 'ccmake ${CMAKE_CURRENT_SOURCE_DIR}' to fine-tune the configuration.")
message(STATUS "Run Sytus: " ${RUN_SYSTUS})
message(STATUS "Run Aster: " ${RUN_ASTER})
message(STATUS "Run Nastran: " ${RUN_NASTRAN})

#
# FILES TO INSTALL
SET(VEGA_EXTENSION)
IF(CYGWIN)
    SET(VEGA_EXTENSION ".exe")
ENDIF(CYGWIN)
# WARNING: WIN32 IS DEPRECATED AND SHOULD NOT BE USED
IF(WIN32)
    SET(VEGA_EXTENSION ".exe")
ENDIF(WIN32)
# PROGRAM
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/bin/vegapp${VEGA_EXTENSION} DESTINATION ${CMAKE_INSTALL_PREFIX})
# LICENCE FILES
INSTALL(FILES LICENSE COPYRIGHT DESTINATION ${CMAKE_INSTALL_PREFIX})
# CUSTOM FILE IF NEEDED.
IF(CMAKE_CUSTOM_INSTALL)
    INCLUDE(${CMAKE_CUSTOM_INSTALL})
ENDIF(CMAKE_CUSTOM_INSTALL)
INSTALL(CODE "MESSAGE(\"VEGA installed in \" ${CMAKE_INSTALL_PREFIX})")
