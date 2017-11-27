# Appoint the version of cmake
cmake_minimum_required(VERSION 3.2)

# Appoint the name of project
PROJECT(leftP2P)

# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)
#从下面那两个路径找mysql.h，将找到的路径放到MYSQL_INCLUDE_DIR这个变量中。
FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
  /usr/local/include/mysql
  /usr/include/mysql
IF (WIN32)
  "C:/Program Files/MySQL/MySQL Server 5.7/include"
ENDIF ()
)
#设置要寻找的链接库的名字，这里应该是找libmysqlclient.so
SET(MYSQL_NAMES mysqlclient mysqlclient_r)
#将libmysqlclient.so的路径放到MYSQL_LIBRARY
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES ${MYSQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

#如果上面找到mysql的头文件和mysql的库文件，就将MYSQL_FOUND设为真
IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

#如果找到并且没有设置QUIET，则输出搜到动态库路径
#如果找不到就输出错误信息
IF (MYSQL_FOUND)
  IF (NOT MYSQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
  ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
  IF (MYSQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

#将他标记为高级变量，具体什么作用没查到
MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE_DIR
  )

IF (WIN32)
	LINK_DIRECTORIES("C:/Program Files/MySQL/MySQL Server 5.7/lib")
ENDIF ()


# Appoint the head file path
INCLUDE_DIRECTORIES(
include
${MYSQL_INCLUDE_DIR}
)

# Appoint the source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(include/CompleteConfidence DIR_CC_SPT)
AUX_SOURCE_DIRECTORY(include/LeftMyCodes DIR_MYCODES_SPT)

# Set environement variable
SET(_MATH
${DIR_SRCS}
${DIR_CC_SPT}
${DIR_MYCODES_SPT}
)

# Add excutable file
ADD_EXECUTABLE(${PROJECT_NAME} ${_MATH})

# Add link library
IF (WIN32)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} libmysql)
ELSEIF (UNIX)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} m ${MYSQL_LIBRARY})
ENDIF ()  

