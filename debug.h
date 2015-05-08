#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define DEBUG 1
//#define DEBUG 0

#if DEBUG == 1
	#define DLog(fmt, ...) printf((ANSI_COLOR_CYAN "%s %s [Line %d]: " fmt ANSI_COLOR_RESET "\n"), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define DLog(...)
#endif

