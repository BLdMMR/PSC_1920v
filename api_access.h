#ifndef APIACCESSAPP_H
#define APIACCESSAPP_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Body {
	const char* name;
	double radius;
	float mass;
	double gravity;
} Body;

typedef struct Mem {
	char * memory;
	size_t size;
}mem_t;

//extern Body bodies[];
#endif
