CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(ai_sdk)

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

LOAD_OPENCV()
LOAD_GLOG()

IF (TARGET_OS STREQUAL "Android")
    LOAD_ANDROID_ENV()
ENDIF()

FILE(GLOB_RECURSE CURRENT_DIR_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(MAKE_DIRECTORY ${PROJECT_INCLUDE_DIR}/ai_sdk)
FOREACH(include ${CURRENT_DIR_HEAD})
	MESSAGE("-- Copying ${include}")
	CONFIGURE_FILE(${include} ${PROJECT_INCLUDE_DIR}/ai_sdk COPYONLY)
ENDFOREACH()

FILE(GLOB_RECURSE CURRENT_DIR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

SET(DEPENDENCY_INCLUDES
	${PROJECT_INCLUDE_DIR} 
	${PROJECT_INCLUDE_DIR}/ai_sdk
	${OpenCV_INCLUDE_DIRS} 
)

SET(DEPENDENCY_LIBS
	core
	module_logger
	glog::glog
	${OpenCV_LIBS}
	${ANDROID_JIN_LIBS}
)

IF(INFER_ENGINE STREQUAL NCNN)
	LOAD_NCNN()
	LIST(APPEND DEPENDENCY_INCLUDES ${NCNN_INCLUDE_DIR})
	LIST(APPEND DEPENDENCY_LIBS ${OpenMP_CXX_FLAGS} ${NCNN_LIBS})
	ADD_COMPILE_OPTIONS(-fopenmp)
ELSEIF(INFER_ENGINE STREQUAL ORT)
	LOAD_ONNXRUNTIME()
	LIST(APPEND DEPENDENCY_INCLUDES ${ONNXRUNTIME_INCLUDE_DIR} ${ONNXRUNTIME_INCLUDE_DIR}/onnxruntime)
	LIST(APPEND DEPENDENCY_LIBS ${ONNXRUNTIME_LIBS})
ELSE()
    MESSAGE(FATAL_ERROR "Invalid inference engine type ${INFER_ENGINE}")
ENDIF()

# Build Shared Library
ADD_LIBRARY(ai_sdk SHARED ${CURRENT_DIR_SRCS})
TARGET_COMPILE_DEFINITIONS(ai_sdk PRIVATE AI_WORKFLOW_BUILD_DLL)

TARGET_INCLUDE_DIRECTORIES(ai_sdk PRIVATE ${DEPENDENCY_INCLUDES})
TARGET_LINK_LIBRARIES(ai_sdk PRIVATE ${DEPENDENCY_LIBS})

SET_TARGET_PROPERTIES(ai_sdk PROPERTIES
	VERSION 1.0.0
	SOVERSION 1
)

INSTALL(TARGETS ai_sdk
    EXPORT ai_sdk-targets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

INSTALL(EXPORT ai_sdk-targets
    NAMESPACE ai_sdk::
    DESTINATION lib/cmake/ai_sdk
)
