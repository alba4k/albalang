#ifndef ERROR_H
#define ERROR_H

#define ERR_BAD_USAGE 50
#define ERR_NO_SUCH_FILE 51
#define ERR_FILE_UNREADABLE 52

#define ERR_GENERIC 60
#define ERR_SYNTAX 61
#define ERR_VAR_NOT_FOUND 62
#define ERR_WRONG_TYPE 63

void error(const char *message, const char *extra, const int code, void *memory);

#endif // ERROR_H