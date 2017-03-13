# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(GRAPH_FCGI_OBJ
        PNG2FCGIStream.cpp
        GraphCommands.cpp
        PieChart.cpp
        GraphFCGIServer.cpp
        GraphFCGIOptions.cpp
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(graph.fcgi ${GRAPH_FCGI_OBJ})
ADD_DEPENDENCIES(graph.fcgi w3tk_shared)
TARGET_LINK_LIBRARIES(graph.fcgi w3tk gd ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})
