# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TMP_TEST_OBJ
        tmp-test.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(tmp-test ${TMP_TEST_OBJ})
ADD_DEPENDENCIES(tmp-test w3tk)
TARGET_LINK_LIBRARIES(tmp-test w3tk ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})
