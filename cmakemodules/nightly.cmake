SET(CTEST_PROJECT_NAME "vegapp")
SET(CTEST_NIGHTLY_START_TIME "00:00:00 EST")
SET (CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
SET (CTEST_BINARY_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/build/x86_64")
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)
SET (WITH_MEMCHECK TRUE)

#10 hours for tests
set (CTEST_TEST_TIMEOUT 36000 CACHE STRING 
    "Maximum time allowed before CTest will kill the test.") 
set (DART_TESTING_TIMEOUT 36000 CACHE STRING 
    "Maximum time allowed before CTest will kill the test." FORCE)

find_program(CTEST_SVN_COMMAND NAMES svn)
set (CTEST_UPDATE_COMMAND ${CTEST_SVN_COMMAND})
find_program(CTEST_CTEST_COMMAND NAMES ctest)
SET (CTEST_COMMAND "${CTEST_CTEST_COMMAND} -D Nightly -j 4")
find_program(CTEST_CMAKE_COMMAND1 NAMES cmake)

find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
#not used anymore
#set (CTEST_CUSTOM_MEMCHECK_IGNORE "Nastran2Aster_nightly")
set (CTEST_MEMORYCHECK_COMMAND_OPTIONS, "--leak-check=full --track-fds=yes")

SET (CTEST_CMAKE_COMMAND "${CTEST_CMAKE_COMMAND1}")
set (CTEST_CMAKE_GENERATOR "Unix Makefiles")
set (CTEST_BUILD_NAME "Linux-c++")
set (CTEST_BUILD_COMMAND "/usr/bin/make -j6")

if($ENV{CTEST_SITE_NAME}) 
set (CTEST_SITE $ENV{CTEST_SITE_NAME} CACHE STRING "CTEST site name") 
else($ENV{CTEST_SITE_NAME}) 
    execute_process(COMMAND hostname OUTPUT_VARIABLE HOSTNAME 
                OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT HOSTNAME)
      set(HOSTNAME "unknown")
    endif(NOT HOSTNAME)
    set (CTEST_SITE ${HOSTNAME} CACHE STRING "CTEST site name")     
endif($ENV{CTEST_SITE_NAME}) 
site_name(${CTEST_SITE})

ctest_start("Nightly")
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
ctest_update(SOURCE "${CTEST_SCRIPT_DIRECTORY}")
ctest_configure( OPTIONS "-DNIGHTLY_BUILD=TRUE")
ctest_build()
ctest_test()
if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
  ctest_coverage()
endif (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
  ctest_memcheck()
endif (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
ctest_submit()
