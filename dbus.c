#include "dbus.h"
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

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

static void _make_body(char* strings[], int params_quantity) {
    for (int i = 0; i < params_quantity; i++) {
        printf("param/method: %s, with length: %ld\n", strings[i], strlen(strings[i]));
    }
    uint32_t size_first_param = strlen(strings[1]);
    uint32_t little_endian = htonl(size_first_param);
    printf("byte 1 = %u\n", (little_endian & 0xFF000000));
    printf("byte 2 = %u\n", (little_endian & 0x00FF0000));
    printf("byte 3 = %u\n", (little_endian & 0x0000FF00));
    printf("byte 4 = %u\n", (little_endian & 0x000000FF)); 
    printf("length: %d\n", little_endian);
    printf("length: %d\n", ntohl(little_endian)); //);


}

char *get_send_message(char *line) {

    //Almacena los argumentos, (ruta, destino, interfaz, metodo(arg1, ..))
    char *args[4] = {0}; 

    if (!_split(line, args, 4, " ", " ", NULL)) return NULL;
    int open_parentheses = _count_chars(args[3], '(');
    int closing_parentheses = _count_chars(args[3], ')');
    if ((open_parentheses != closing_parentheses) || (open_parentheses > 1)) return NULL;
    int params_quantity = 1;
    if (open_parentheses == 1) params_quantity = _count_chars(args[3], ',') + 1 + params_quantity;
    char *method[params_quantity];
    if (!_split(args[3], method, params_quantity, "(", ",", ")"));
    if (params_quantity > 1) {
        _make_body(method, params_quantity);
    }
    // printf("ruta: %s\n", args[0]);
    // printf("destino: %s\n", args[1]);
    // printf("interfaz: %s\n", args[2]);

    return "HOLA"; 
}
