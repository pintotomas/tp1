#include "dbus.h"
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
/* Separa los strings en line y los guarda en el array strings.
   Se utiliza por primera vez el delimitador delim1, y luego delim2 hasta finalizar el recorrido del string.
   si se pasa por parametro delim3, se aplicara antes de iniciar los demas splits
 */
static bool _split(char *line, char* strings[], size_t size, char *delim1, char *delim2, char *delim3) {
    
    char *token = line;
    if (delim3 != NULL) {
        token = strtok(line, delim3);
    } 
    token = strtok(token, delim1); 
    int i = 0;
    strings[i] = token;
    i++;
    while (i < size) { 
        token = strtok(NULL, delim2);
        if (token == NULL) return false;
        strings[i] = token;
        i++;

    } 

    return true;
}

static int _count_chars(const char* string, char ch)
{
    int count = 0;
    int i;
    int length = strlen(string);

    for (i = 0; i < length; i++)
    {
        if (string[i] == ch)
        {
            count++;
        }
    }

    return count;
}

int _calculate_body_length(char* strings[], int params_quantity) {
    int body_total_length = 0;
    for (int i= 1; i < params_quantity; i++) {
        //Por cada parametro necesito 4 bytes p/ header y luego el str y el 00
        body_total_length += 4 + strlen(strings[i]) + 1;
    }    
    return body_total_length;
}   

unsigned char* _make_body(char* strings[], int params_quantity, int body_length) {

    // int body_total_length = 0;
    // for (int i= 1; i < params_quantity; i++) {
    //     //Por cada parametro necesito 4 bytes p/ header y luego el str y el 00
    //     body_total_length += 4 + strlen(strings[i]) + 1;
    // }   
    unsigned char body[body_length];
    unsigned char *s_ptr;
    //unsigned char *body = malloc(sizeof(unsigned char) * (body_length));  

    int body_position = 0;
    for (int k= 1; k < params_quantity; k++) {
        uint32_t bytes_quantity = strlen(strings[k]);
        //memcpy(&body[body_position], &bytes_quantity, 4);
        memcpy(&body[body_position], &bytes_quantity, 4);
        body_position += 4;
        memcpy(&body[body_position], strings[k], strlen(strings[k]));
        body_position += strlen(strings[k]);
        memcpy(&body[body_position], "\0", 1);
        body_position += 1;

        //memcpy(dest, src, size)
        // for (int j = 0, j < strlen(strings[i]); j++) {
        //     body[body_position] = strings[i][j];
        // } 
    } 
    //self->body = s_ptr;
    for (int j = 0; j < body_length; j++) {
        printf("Current byte: %x\n", body[j]);   
        //printf("Current byte: %x\n", body[j]);    
    }
    s_ptr = malloc(sizeof(unsigned char) * body_length);
    memcpy(s_ptr, body, body_length);


    return s_ptr;   
    //self->body_length = body_length;
    // uint32_t size_first_param = strlen(strings[1]);
    // uint32_t little_endian = htonl(size_first_param);
    // printf("byte 1 = %u\n", (little_endian & 0xFF000000));
    // printf("byte 2 = %u\n", (little_endian & 0x00FF0000));
    // printf("byte 3 = %u\n", (little_endian & 0x0000FF00));
    // printf("byte 4 = %u\n", (little_endian & 0x000000FF)); 
    // printf("length: %d\n", little_endian);
    // printf("length: %d\n", ntohl(little_endian)); //);


}

unsigned char* create_send_message(char *line, int *body_length) {

    //Almacena los argumentos, (ruta, destino, interfaz, metodo(arg1, ..))
    char *args[4] = {0}; 
    unsigned char *body = NULL;
    int body_length_calc = 0;
    if (!_split(line, args, 4, " ", " ", NULL)) return false;
    int open_parentheses = _count_chars(args[3], '(');
    int closing_parentheses = _count_chars(args[3], ')');
    if ((open_parentheses != closing_parentheses) || (open_parentheses > 1)) return false;
    int params_quantity = 1;
    if (open_parentheses == 1) params_quantity = _count_chars(args[3], ',') + 1 + params_quantity;
    char *method[params_quantity];
    if (!_split(args[3], method, params_quantity, "(", ",", ")"));
    if (params_quantity > 1) {
        body_length_calc = _calculate_body_length(method, params_quantity);
        body = _make_body(method, params_quantity, body_length_calc);
    }

    *body_length = body_length_calc;
    // for (int j = 0; j < body_length_calc; j++) {
    //     printf("Current byte: %x\n", body[j]);   
    //     //printf("Current byte: %x\n", body[j]);    
    // } 
    // printf("ruta: %s\n", args[0]);
    // printf("destino: %s\n", args[1]);
    // printf("interfaz: %s\n", args[2]);

    return body;
}
