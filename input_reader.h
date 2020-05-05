#ifndef _INPUT_READER_H
#define _INPUT_READER_H

#include <stdio.h>
#include <stdbool.h>
#define READ_SIZE 32

typedef struct {
	FILE *input;
	int position; //position in bytes
} input_reader_t;

// Returns false if anything fails at init
// Opens FILE filename to read from it, or uses STDIN if filename is NULL
bool input_reader_init(input_reader_t *self, char *filename);

// Returns a char* malloc'd that user should free, or NULL if there are no new lines
char* input_reader_get_next_line(input_reader_t *self);

//Close input stream
void input_reader_destroy(input_reader_t *self);

#endif
