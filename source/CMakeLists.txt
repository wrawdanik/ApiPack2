

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/libraries" CACHE PATH "default install path" FORCE )
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/source/utils/

)
 

file(   
	 GLOB SOURCES
	"${CMAKE_SOURCE_DIR}/source/datastore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/memorystore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/pipeline/*.cpp"
)


add_executable(ApiPack2 ${SOURCES} memorystore/CacheStatistics.h)

link_directories(/usr/lib/)
include_directories(/usr/include/postgresql/)
target_link_libraries (ApiPack2 libpq.so )
