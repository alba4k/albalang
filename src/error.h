#pragma once

#ifndef ERROR_H
#define ERROR_H

// messages functions can return
#define RET_OK 00
#define RET_CONTINUE 01
#define RET_BREAK 02

// not related to albalang
#define ERR_BAD_USAGE 50
#define ERR_NO_SUCH_FILE 51
#define ERR_FILE_UNREADABLE 52
#define ERR_OOM 53

// related to syntax
#define ERR_GENERIC 60
#define ERR_SYNTAX 61
#define ERR_ARGC 62

// related to variables
#define ERR_VAR_NOT_FOUND 70
#define ERR_WRONG_TYPE 71

// related to lists
#define ERR_LIST_NOT_FOUND 80

void error(const char *message, const char *extra, const int code, void *memory);

#endif // ERROR_H