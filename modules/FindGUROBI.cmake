#### Taken from http://www.openflipper.org/svnrepo/CoMISo/trunk/CoMISo/cmake/FindGUROBI.cmake

# in case of trouble think about recompiling the gurobi C++ interface ....
# - Try to find GUROBI
# Once done this will define
#  GUROBI_FOUND - System has Gurobi
#  GUROBI_INCLUDE_DIRS - The Gurobi include directories
#  GUROBI_LIBRARIES - The libraries needed to use Gurobi



find_path(GUROBI_INCLUDE_DIR 
          NAMES gurobi_c++.h
          PATHS "$ENV{GUROBI_HOME}/include"
          )

find_library( GUROBI_LIBRARY 
              NAMES gurobi
		    gurobi45
		    gurobi46
		    gurobi50 
		    gurobi51
		    gurobi52
		    gurobi55
		    gurobi56
		    gurobi60
		    gurobi65
		    gurobi81
		    gurobi91
		    PATHS "$ENV{GUROBI_HOME}/lib"
              )

find_library( GUROBI_CXX_LIBRARY 
              NAMES gurobi_c++
              PATHS "$ENV{GUROBI_HOME}/lib"
              )

set(GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}" )
#set(GUROBI_LIBRARIES "${GUROBI_LIBRARY};${GUROBI_CXX_LIBRARY}" )
set(GUROBI_LIBRARIES "${GUROBI_CXX_LIBRARY};${GUROBI_LIBRARY}")

# use c++ headers as default
# set(GUROBI_COMPILER_FLAGS "-DIL_STD" CACHE STRING "Gurobi Compiler Flags")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GUROBI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GUROBI  DEFAULT_MSG
                                  GUROBI_LIBRARY GUROBI_CXX_LIBRARY GUROBI_INCLUDE_DIR)

mark_as_advanced(GUROBI_INCLUDE_DIR GUROBI_LIBRARY GUROBI_CXX_LIBRARY)

