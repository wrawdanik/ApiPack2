

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/libraries" CACHE PATH "default install path" FORCE )
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/source/utils/
		${CMAKE_SOURCE_DIR}/source/handler/
		${CMAKE_SOURCE_DIR}/thrift/gen-cpp/
)
 

file(   
	 GLOB SOURCES
		"${CMAKE_SOURCE_DIR}/thrift/gen-cpp/*.cpp"
	"${CMAKE_SOURCE_DIR}/source/datastore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/memorystore/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/pipeline/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/handler/*.cpp"
		"${CMAKE_SOURCE_DIR}/source/thriftHelpers/*.h"
)


add_executable(ApiPack2 ${SOURCES} handler/ApiPack2HandlerFactory.cpp handler/ApiPack2HandlerFactory.h handler/LogicHandler.cpp handler/LogicHandler.h datastore/SerializedDataObject.cpp datastore/SerializedDataObject.h thriftHelpers/ThriftModelOperators.cpp)

link_directories(/usr/lib/ /usr/local/lib)
include_directories(/usr/include/postgresql/  /usr/local/include)
target_link_libraries (ApiPack2 libpq.so proxygenhttpserver thrift wangle folly z ssl crypto cap event gflags glog pthread )
