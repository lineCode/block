PROJECT(core)

#引用头文件目录
if(WIN32)
	INCLUDE_DIRECTORIES(
		${BLOCK_SOURCE_DIR}/3rd/fmt
		${BLOCK_SOURCE_DIR}/3rd/glog/src
	)
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS}")

	INCLUDE_DIRECTORIES(
		${BLOCK_SOURCE_DIR}/lib/glog/include
		${BLOCK_SOURCE_DIR}/lib/fmt/include
	)
endif()

if (APPLE)
	message("APPLE")
	INCLUDE_DIRECTORIES(
			${BLOCK_SOURCE_DIR}/3rd/fmt
			/usr/local/Cellar/gflags/2.2.1/include
			/usr/local/Cellar/glog/0.3.5_3/include
	)
endif()

Add_Definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")

#引用库目录
LINK_DIRECTORIES(
)

FILE(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")
ADD_LIBRARY(core STATIC ${SRC})

if(MSVC)     
    # Use the static C library for all build types
    foreach(var 
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
      )
      if(${var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
      endif()
    endforeach()    
    
    SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/win)
elseif(UNIX)
	SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
elseif(APPLE)
    SET(LIBRARY_OUTPUT_PATH ${BLOCK_SOURCE_DIR}/lib/)
endif()

