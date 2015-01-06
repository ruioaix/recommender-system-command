/**
 * This file may be included by any other *.c or *.h file.
 * it provide functions and preprocessor macros that could be used most commonly by  other functions.
 *
 * Author: RuiXiao <xrfind@gmail.com>
 */
#ifndef CNRT_BASE_H
#define CNRT_BASE_H

//#define NDEBUG

/****************VERBOSE LEVEL*******************************************************************/
//LEVEL <   5 : all functions defined here will be extended to ((void)0).
//LEVEL >=  5 : printer will output.
//LEVEL >= 10 : printer printgf will output.
//LEVEL >= 20 : printer printgf printsf will output.
//LEVEL >= 30 : printer printgf printsf printgfl will output.
//LEVEL >= 40 : printer printgf printsf printgfl printsfl will output.
#define VERBOSE_LEVEL 40
#define printer(format, ...) ((void)0)
#define printgf(format, ...) ((void)0)
#define printgfl(format, ...) ((void)0)
#define printsf(format, ...) ((void)0)
#define printsfl(format, ...) ((void)0)

#if VERBOSE_LEVEL >= 5
#undef printer
#define printer(format, ...) do {\
		fflush(stdout);\
		fprintf(stderr, "[E] ");\
		fprintf(stderr, "%s =>> ", __func__);\
		fprintf(stderr, format, ##__VA_ARGS__);\
		fprintf(stderr, "\n");\
} while(0)
#if VERBOSE_LEVEL >= 10
#undef printgf
#define printgf(format, ...) do {\
		printf("[LEVLE %d] ", VERBOSE_LEVEL);\
		printf("%s =>> ", __func__);\
		printf(format, ##__VA_ARGS__);\
		printf("\n");\
} while(0)
#if VERBOSE_LEVEL >= 20
#undef printsf
#define printsf(format, ...) do {\
		printf("[LEVLE %d] ", VERBOSE_LEVEL);\
		printf("%s ==> ", __func__);\
		printf(format, ##__VA_ARGS__);\
		printf("\n");\
} while(0)
#if VERBOSE_LEVEL >= 30
#undef printgfl
#define printgfl(format, ...) do {\
		printf("[LEVLE %d] ", VERBOSE_LEVEL);\
		printf("%s ==> ", __func__);\
		printf(format, ##__VA_ARGS__);\
		printf("\n");\
} while(0)
#if VERBOSE_LEVEL >= 40
#undef printsfl
#define printsfl(format, ...) do {\
		printf("[LEVLE %d] ", VERBOSE_LEVEL);\
		printf("%s ==> ", __func__);\
		printf(format, ##__VA_ARGS__);\
		printf("\n");\
} while(0)
#endif
#endif
#endif
#endif
#endif
/********************************************************************************************************/


/********assert should be used very offen.***************************************************************/
#include <assert.h>
/********************************************************************************************************/

/********use everywhere.*********************************************************************************/
void print_time(void);
void print_label(int i);
/********************************************************************************************************/

/*******use everywhere.**********************************************************************************/
#include <stdio.h> //for FILE, perror, fprintf, stderr
void iserror(char *format, ...);
#define isError(format, ...) do {\
		fflush(stdout);\
		fprintf(stderr, "[ERROR]:\n\tfile: \"%s\", line: %d.\n\t%s =>> ", \
				__FILE__, __LINE__, __func__);\
		iserror(format, ##__VA_ARGS__);\
} while(0)
/********************************************************************************************************/

/*******some time, I use*********************************************************************************/
double dmin(double a, double b);
double dmax(double a, double b);
int imin(int a, int b);
int imax(int a, int b);
long lmin(long a, long b);
long lmax(long a, long b);
/********************************************************************************************************/

/*********use everywhere.********************************************************************************/
void *malloc_safe(size_t size, const char *funcname, const char *filename, const int lineNum);
void *calloc_safe(size_t num, size_t size, const char *funcname, const char *filename, const int lineNum);
void *realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum);
#define smalloc(n) malloc_safe(n, __func__, __FILE__, __LINE__)
#define scalloc(n, s) calloc_safe(n, s, __func__, __FILE__, __LINE__)
#define srealloc(p, n) realloc_safe(p, n, __func__, __FILE__, __LINE__)

FILE *fopen_safe(const char *openfilename, const char *mode, const char *functionname, char *filename, const int lineNum);
#define sfopen(fn, mode) fopen_safe(fn, mode, __func__, __FILE__, __LINE__)
/********************************************************************************************************/

#endif
