set(CTEST_PROJECT_NAME "vegapp")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 0)

set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE_CDASH TRUE)

set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=Vegapp")

