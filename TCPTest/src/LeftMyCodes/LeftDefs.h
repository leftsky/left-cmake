
#ifndef _LEFT_DEFS_H_
#define _LEFT_DEFS_H_

#ifdef _WIN32
#define LEFT_OS_WIN
#endif
#ifdef linux
#define LEFT_OS_LINUX				1991
#endif


#define LEFT_ERROR					unsigned long
// 通用错误
#define LEFT_SUCCESS				0x0L
#define LEFT_ERROR_LEN				0x1L
#define LEFT_ERROR_ARGVS			0x2L
#define LEFT_ERROR_ACCIDENT			0x3L
// 文件读写错误
#define LEFT_ERROR_IO_FILEOPEN		0xFFFE0001L
// IniInfo系列宏和错误代码
#define MAX_INI_INFO_LEN			128
#define LEFT_ERROR_INIINFO_NOFOUND	0xFFFD0001L


#endif
