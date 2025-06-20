CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(core)

SET(CMAKE_POSITION_INDEPENDENT_CODE ON) 

LOAD_OPENCV()
LOAD_CRYPTOPP()
LOAD_GLOG()

SET(DEPENDENCY_INCLUDES
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_INCLUDE_DIR}/core
	${OpenCV_INCLUDE_DIRS}
)

SET(DEPENDENCY_LIBS
	${OpenCV_LIBS}
	cryptopp::cryptopp
	module_logger
	glog::glog
)

FILE(GLOB CURRENT_DIR_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
FILE(GLOB CURRENT_DIR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

IF(INFER_ENGINE STREQUAL NCNN)
	LOAD_OPENMP()
	LOAD_NCNN()
	LIST(APPEND DEPENDENCY_INCLUDES ${NCNN_INCLUDE_DIR})
	LIST(APPEND DEPENDENCY_LIBS ${OpenMP_CXX_FLAGS} ${NCNN_LIBS})
	ADD_COMPILE_OPTIONS(-fopenmp)

	FILE(GLOB NCNN_DIR_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/ncnn/*.hpp)
	LIST(APPEND CURRENT_DIR_HEAD ${NCNN_DIR_HEAD})
	FILE(GLOB NCNN_DIR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ncnn/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ncnn/*.cc)
	LIST(APPEND CURRENT_DIR_SRCS ${NCNN_DIR_SRCS})
	MESSAGE(INFO "Inference engine type is NCNN")

ELSEIF(INFER_ENGINE STREQUAL ORT)
	LOAD_ONNXRUNTIME()
	LIST(APPEND DEPENDENCY_LIBS ${ONNXRUNTIME_LIBS})
	LIST(APPEND DEPENDENCY_INCLUDES ${ONNXRUNTIME_INCLUDE_DIR} ${ONNXRUNTIME_INCLUDE_DIR}/onnxruntime)

	FILE(GLOB_RECURSE ORT_DIR_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/ort/*.hpp)
	LIST(APPEND CURRENT_DIR_HEAD ${ORT_DIR_HEAD})
	FILE(GLOB_RECURSE ORT_DIR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ort/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ort/*.cc)
	LIST(APPEND CURRENT_DIR_SRCS ${ORT_DIR_SRCS})

	IF (TARGET_OS STREQUAL "Android")
		ADD_DEFINITIONS(-DUSE_QNN)
	ENDIF()

	MESSAGE(INFO "Inference engine type is ONNXRuntime")
ELSE()
	MESSAGE(FATAL_ERROR "Invalid inference engine type ${INFER_ENGINE}")
ENDIF()

FILE(MAKE_DIRECTORY ${PROJECT_INCLUDE_DIR}/core)
FOREACH(include ${CURRENT_DIR_HEAD})
	MESSAGE("-- Copying ${include}")
	CONFIGURE_FILE(${include} ${PROJECT_INCLUDE_DIR}/core COPYONLY)
ENDFOREACH()

ADD_LIBRARY(core ${CURRENT_DIR_SRCS})
TARGET_INCLUDE_DIRECTORIES(core PRIVATE ${DEPENDENCY_INCLUDES})
TARGET_LINK_LIBRARIES(core PRIVATE ${DEPENDENCY_LIBS})
TARGET_COMPILE_OPTIONS(core PRIVATE -fopenmp)

# FIXME: write it as a macro for now. it would be more reasonable to change it to read from a certain file
SET(COMMIT_CODE "689bc3e3bdf1c5f2cff81725011ba7d3c0089b25")
TARGET_COMPILE_DEFINITIONS(core PRIVATE SECURITY_KEY="${COMMIT_CODE}")
