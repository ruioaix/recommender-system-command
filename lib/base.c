#include "base.h"

/********************************************************************************************************/
#include <time.h>
void print_time(void) {
	time_t t=time(NULL); 
	printf("time: %s", ctime(&t)); 
	fflush(stdout);
}
void print_label(int i) {
	printf("xxxx==%d==xxxx\n", i);
	fflush(stdout);
}
/********************************************************************************************************/

/********************************************************************************************************/
#include <stddef.h> //for NULL
#include <stdlib.h> //for exit & EXIT_FAILURE
#include <stdarg.h>
void iserror(char *format, ...) {
	fflush(stdout);
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}
/********************************************************************************************************/

/********************************************************************************************************/
inline double dmin(double a, double b) {
	return a<b?a:b;
}
inline double dmax(double a, double b) {
	return a>b?a:b;
}
inline int imin(int a, int b) {
	return a<b?a:b;
}
inline int imax(int a, int b) {
	return a>b?a:b;
}
inline long lmin(long a, long b) {
	return a<b?a:b;
}
inline long lmax(long a, long b) {
	return a>b?a:b;
}
/********************************************************************************************************/

/********************************************************************************************************/
void *malloc_safe(size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = malloc(size);
	if (tmp == NULL) {
		fprintf(stderr, "[ERROR]:\n\t%s => malloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
		exit(EXIT_FAILURE);
	}
	return tmp;
}
void *calloc_safe(size_t num, size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = calloc(num, size);
	if (tmp == NULL) {
		fprintf(stderr, "[ERROR]:\n\t%s => calloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
		exit(EXIT_FAILURE);
	}
	return tmp;
}
void *realloc_safe(void *p, size_t size, const char *funcname, const char *filename, const int lineNum) {
	void *tmp = realloc(p, size);
	if (tmp == NULL) {
		fprintf(stderr, "[ERROR]:\n\t%s => realloc failed: \"%s\" file, %d line.\n", funcname, filename, lineNum);
		exit(EXIT_FAILURE);
	}
	return tmp;
}

FILE *fopen_safe(const char *openfilename, const char *mode, const char *funcname, char *filename, const int lineNum) {
	FILE *fp = fopen(openfilename, mode);
	if (fp == NULL) {
		fprintf(stderr, "[ERROR]:\n\t%s => fopen failed: can not open \"%s\" file, location: \"%s\", %d line.\n", funcname, openfilename, filename, lineNum);
		exit(EXIT_FAILURE);
	}
	return fp;
}
/********************************************************************************************************/
