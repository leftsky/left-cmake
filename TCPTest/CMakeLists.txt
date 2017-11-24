# Appoint the version of cmake
cmake_minimum_required(VERSION 3.2)

# Appoint the name of project
PROJECT(TCPTest)

# Appoint the head file path
INCLUDE_DIRECTORIES(
include
)

# Appoint the source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(src/LeftMyCodes DIR_MYCODES)
AUX_SOURCE_DIRECTORY(src/CompleteConfidence DIR_CC)

# Set environement variable
SET(_MATH
${DIR_SRCS}
${DIR_MYCODES}
${DIR_CC}
)

# Add excutable file
ADD_EXECUTABLE(${PROJECT_NAME} ${_MATH})

# Add link library
IF (WIN32)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME})
ELSEIF (UNIX)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} m pthread)
ENDIF ()  

