# ==============================================================================
# w3tk - WWW ToolKit CMake File
# ==============================================================================

# this is required for scimafic library ----------------------------------------
INCLUDE_DIRECTORIES(lib/w3tk SYSTEM)
INCLUDE_DIRECTORIES(lib/w3tk/fastcgi SYSTEM)
INCLUDE_DIRECTORIES(lib/w3tk/template SYSTEM)

# include subdirectories -------------------------------------------------------
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(test)
