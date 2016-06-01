

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/libraries" CACHE PATH "default install path" FORCE )
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/source/utils/
		${CMAKE_SOURCE_DIR}/source/handler/
		${CMAKE_SOURCE_DIR}/thrift/gen-cpp/
		${CMAKE_SOURCE_DIR}/source/datastore/
		${CMAKE_SOURCE_DIR}/source/diskstore/
)
 

file(   
	 	GLOB DATASTORE_SOURCES
		"${CMAKE_SOURCE_DIR}/thrift/gen-cpp/core/*.cpp"
		"${CMAKE_SOURCE_DIR}/thrift/gen-cpp/baseline/*.cpp"
		"${CMAKE_SOURCE_DIR}/thrift/gen-cpp/TMSId-Programs/*.cpp"
		"${CMAKE_SOURCE_DIR}/thrift/gen-cpp/merckx/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/datastore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/thriftHelpers/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/utils/*.cpp"
)

add_library(Datastore STATIC ${DATASTORE_SOURCES} )

file(
		GLOB SERVER_SOURCES
		"${CMAKE_SOURCE_DIR}/source/server/main.cpp"
		"${CMAKE_SOURCE_DIR}/source/memorystore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/pipeline/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/handler/*.cpp"
)



file(
		GLOB DATA_DUMP_SOURCES
		"${CMAKE_SOURCE_DIR}/source/tools/dataDump/*.cpp"
)




link_directories(/usr/lib/ /usr/local/lib)
include_directories(/usr/include/postgresql/  /usr/local/include)

add_executable(apiPack2 ${SERVER_SOURCES} )
target_link_libraries (apiPack2 libpq.so proxygenhttpserver thrift wangle folly libdouble-conversion.so z ssl crypto cap event gflags glog pthread )

add_executable(dataDump ${DATA_DUMP_SOURCES} )
target_link_libraries (dataDump Datastore libpq.so thrift wangle folly libdouble-conversion.so z ssl crypto cap event gflags glog pthread boost_date_time)
