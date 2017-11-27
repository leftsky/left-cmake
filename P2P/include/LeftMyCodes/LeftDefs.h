
#ifndef _LEFT_DEFS_H_
#define _LEFT_DEFS_H_

#ifdef _WIN32
#define LEFT_OS_WIN
#endif
#ifdef linux
#define LEFT_OS_LINUX
#endif


#define LEFT_ERROR			int

#define LEFT_SUCCESS		0x0L
#define LEFT_ERROR_LEN		0x1L

#endif
