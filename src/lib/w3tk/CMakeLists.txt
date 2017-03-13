# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(W3TK_OBJ
    W3TKMainHeader.cpp

    # FastCGI
    fastcgi/FCGIProtocol.cpp
    fastcgi/FCGIStream.cpp
    fastcgi/FCGIParams.cpp
    fastcgi/FCGIRequest.cpp
    fastcgi/FCGIServer.cpp

    # templates
    template/TemplateParamItem.cpp
    template/TemplateParamNode.cpp
    template/TemplateParams.cpp
    template/Template.cpp
    template/TemplateCache.cpp
    template/TemplatePreprocessor.cpp
    )

# create static library --------------------------------------------------------
IF(LIBS_STATIC)
    ADD_LIBRARY(w3tk_static STATIC ${W3TK_OBJ})
    SET_TARGET_PROPERTIES(w3tk_static PROPERTIES
                            OUTPUT_NAME w3tk
                            CLEAN_DIRECT_OUTPUT 1)
ENDIF(LIBS_STATIC)

# create shared library --------------------------------------------------------
IF(LIBS_SHARED)
    ADD_LIBRARY(w3tk_shared SHARED ${W3TK_OBJ})
    TARGET_LINK_LIBRARIES(w3tk_shared hipoly) 
    SET_TARGET_PROPERTIES(w3tk_shared PROPERTIES
                            OUTPUT_NAME w3tk
                            CLEAN_DIRECT_OUTPUT 1
                            VERSION ${W3TK_VERSION})
    INSTALL(TARGETS
                w3tk_shared
            LIBRARY DESTINATION
                lib)
ENDIF(LIBS_SHARED)

