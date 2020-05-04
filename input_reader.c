#include "input_reader.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

bool input_reader_init(input_reader_t *self, char *filename) {
	if (filename != NULL) self->input = fopen(filename, "rb"); 
    else self->input = stdin;	
    if(self->input != NULL) {
    	return true;
    }
    fprintf(stderr, "fopen: %s\n", strerror(errno));
    return false;
}

void input_reader_destroy(input_reader_t *self) {
	fclose(self->input);
}

static ssize_t _my_getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream) {
    
    if (lineptr == NULL || n == NULL || stream == NULL) {
        fprintf(stderr, "_my_getline, Argumentos incorrectos (Null pointers).\n");
        return -1;
    }

    //Para la lectura de fgets, lo hago de a 32 bytes
    char chunk[32] = {0};

    if (*lineptr == NULL) {
        *n = sizeof(chunk);
        if ((*lineptr = malloc(*n)) == NULL) {
            fprintf(stderr, "No se pudo alocar memoria para la linea.\n");
            return -1;
        }
    }
    (*lineptr)[0] = '\0';
    while (fgets(chunk, sizeof(chunk), stream) != NULL) {
        if (*n - strlen(*lineptr) < sizeof(chunk)) {
            *n *= 2; //resize factor
            if ((*lineptr = realloc(*lineptr, *n)) == NULL) {
                fprintf(stderr, "No se pudo reaalocar memoria para la linea.\n");
                free(lineptr);
                return -1;
            }
        }

        strcat(*lineptr, chunk);

        if ((*lineptr)[strlen(*lineptr) - 1] == '\n') {
            return strlen(*lineptr);
        }
    }
    return -1;
}
char* input_reader_get_next_line(input_reader_t *self) {

    char *line = {0};
    size_t len = 0;
    int next_line_result = _my_getline(&line, &len, self->input);
    if (next_line_result == -1) { // No hay mas lineas
        if (line != NULL) {
            free(line);
        }
        return NULL;
    }
    // for (int i = 0; i < strlen(line); i++) {
    //     printf("%02X\n", (unsigned char)line[i]);
    // }

    return line;
}