#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <nmmintrin.h>

typedef unsigned char uchar;

#define CALLOC(STRUCT_NAME)				(STRUCT_NAME*)calloc(1,   sizeof(STRUCT_NAME))
#define CALLOC_SIZE(CNT, STRUCT_NAME)	(STRUCT_NAME*)calloc(CNT, sizeof(STRUCT_NAME))

const 	float 	LOAD_FACTOR = 10.0;
const 	int 	KNUTH_CONST = 2654435769;

typedef struct Node {
	char* key;
	Node* next;
} Node;

typedef struct Table {
	int cnt;
	int bit_size;
	int (*hash_func)(char*, int);
	Node** buckets;
} Table;

Table*	TableCtor	(int	bit_size, int (*func)(char*, int))	;
void	TableDtor	(Table*	table)								;

void	TableInsert	(Table*	table, char* key)		;
void	TableRehash	(Table* table)					;
int		EasyInsert	(Node* bucket, char* key)		;
void	TablePrint	(Table* table)					;
int		TableFind	(Table* table, char* key)		;

Node*	NodeCtor	(char*	key)					;
void	NodeDtor	(Node* node)					;
int		get_hash	(char*	key, 	int bit_size)	;

#endif
