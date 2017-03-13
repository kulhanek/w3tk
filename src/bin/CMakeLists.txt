# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

IF(GD_FOUND)
    ADD_SUBDIRECTORY(graph-fcgi)
ENDIF(GD_FOUND)
ADD_SUBDIRECTORY(test-fcgi)
