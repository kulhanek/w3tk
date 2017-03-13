# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TEST_FCGI_OBJ
        fcgiserver.cpp
        test-fcgi.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(test.fcgi ${TEST_FCGI_OBJ})
ADD_DEPENDENCIES(test.fcgi w3tk_shared)
TARGET_LINK_LIBRARIES(test.fcgi w3tk ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})
