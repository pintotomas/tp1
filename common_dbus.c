#include "dbus.h"
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
/* Separa los strings en line y los guarda en el array strings.
   Se utiliza por primera vez el delimitador delim1, y luego delim2 hasta finalizar el recorrido del string.
   si se pasa por parametro delim3, se aplicara antes de iniciar los demas splits
 */

int _count_chars(const char* str, char ch)
{
    int count = 0;
    int i;
    int length = strlen(str);
    for (i = 0; i < length; i++)
    {
        if (str[i] == ch)
        {
            count++;
        }
    }
    return count;
}

void _calculate_body_length(dbus_encoder_t *self, char* strings[],
                             int params_quantity) {
    int body_total_length = 0;
    for (int i= 0; i < params_quantity; i++) {
        //Por cada parametro necesito 4 bytes p/ header y luego el str y el 00
        body_total_length += 4 + strlen(strings[i]) + 1;
    }    
    self->body_length = body_total_length;
}   

void _make_body(dbus_encoder_t *self, char* strings[], int params_quantity) {
    unsigned char body[self->body_length];
    int body_position = 0;
    for (int k= 0; k < params_quantity; k++) {
        uint32_t bytes_quantity = strlen(strings[k]);
        memcpy(&body[body_position], &bytes_quantity, 4);
        body_position += 4;
        memcpy(&body[body_position], strings[k], strlen(strings[k]));
        body_position += strlen(strings[k]);
        memcpy(&body[body_position], "\0", 1);
        body_position += 1;
    } 
    self->body = malloc(sizeof(unsigned char) * self->body_length);
    memcpy(self->body, body, self->body_length);
}

//Devuelve el numero mas cercano de n que es multiplo de 8 (Mayor o igual a n)
int _get_closest_multiply(int n, int multiply) {
    int x = 0;
    if ((n % multiply) == 0) {
        x = n;
    } else {
        x = n + (8 - n % 8);
    }
    return x;
}
void _calculate_header_size(dbus_encoder_t *self, char* params[],
                           int args_quantity, int params_quantity) {
    //16 bytes fijos 
    int header_size = 16; 
    for (int i = 0; i < args_quantity; i++) {
        //4 bytes descripcion + 4 bytes longitud 
        header_size += 8; 
        //long de la descripcion + 1 por /0
        int z = strlen(params[i]) + 1;
        //Alineo a 8 bytes
        int x = _get_closest_multiply(z, 8);
        header_size += x;
    }
    if (params_quantity > 0) {
        int bytes_firma = 0;
        bytes_firma += 4; //4 bytes para describir (3 y un 00)  
        //1 byte entero c/cantidad de params, \0 y ademas 
        //1 's' por cada parametro (1 byte x cada param)
        //Resto 1 porque se esta contando tambien el nombre del metodo
        bytes_firma += 2 + params_quantity; 
        // Recordar que cuando especificamos la longitud en el header
        // No debemos contar los bytes que vienen de alinear la firma
        // (O el ultimo)
        header_size += _get_closest_multiply(bytes_firma, 8);
    }
    self->header_length = header_size;
}

/* Genera un header de largo header_size bytes 
params debe estar en el orden <destino> <path> <interfaz> <metodo>
*/
void _create_header(dbus_encoder_t *self, char* params[],
                    int args_quantity, int params_q) {
    unsigned char header[self->header_length];
    uint32_t body_length32 = ((uint32_t) self->body_length);
    unsigned char h1[16] = {0x6c, 0x01, 0x00, 0x01};
    int header_position = 0;
    memcpy(&header[header_position], &h1, 4);
    header_position += 4;
    memcpy(&header[header_position], &body_length32, 4);
    header_position += 4;
    uint32_t id = (uint32_t) self->message_id;
    memcpy(&header[header_position], &id, 4);
    header_position += 4;
    //Posicion del header size: 4+4+4: 12
    unsigned char dummy_header_length[16] = {0xff, 0xff, 0xff, 0xff};
    memcpy(&header[header_position], &dummy_header_length, 4);
    header_position += 4;
    //array con las descripciones de cada parametro a enviar.
    unsigned char array[4][4] =
{ 
    { 0x01, 0x01, 0x6f, 0x00 },
    { 0x06, 0x01, 0x73, 0x00 },
    { 0x02, 0x01, 0x73, 0x00 },
    { 0x03, 0x01, 0x73, 0x00 }
};
    int last_padding = 0;
    for (int u = 0; u < args_quantity; u++) {
        last_padding = _get_closest_multiply(strlen(params[u]) + 1, 8)
                      - strlen(params[u]);
        memcpy(&header[header_position], &array[u], 4);
        header_position += 4;
        uint32_t len = (uint32_t) strlen(params[u]);
        memcpy(&header[header_position], &len, 4);
        header_position += 4;
        memcpy(&header[header_position], params[u], strlen(params[u]));
        header_position += strlen(params[u]);
        for (int k = 0; k < last_padding; k++) {
            memcpy(&header[header_position], "\0", 1);
            header_position++;
        }
    }

    if (params_q > 0) {
        // 4 bytes de header, 1 p/cant de params 1 's' por cada
        // param, y un /0 al final (Por eso el +6).
        // Completa con /0 para alinear.
        unsigned char h_params[16] = {0x09, 0x01, 0x67, 0x00};
        memcpy(&header[header_position], &h_params, 4);
        header_position += 4;
        unsigned char uc_params_q = (unsigned char) params_q;
        memcpy(&header[header_position], &uc_params_q, 1);
        header_position ++;
        for (int z = 0; z < uc_params_q; z++) {
            memcpy(&header[header_position], "s", 1);
            header_position++;
        }
        last_padding = _get_closest_multiply(params_q + 6, 8) - (params_q + 5);
        for (int k = 0; k < last_padding; k++) {
            memcpy(&header[header_position], "\0", 1);
            header_position++;
        }
    }
    uint32_t real_header_length = self->header_length;
    if (last_padding > 1) {
        real_header_length -= last_padding;
        real_header_length++; //Siempre tiene que haber un /0 al final.
    }
    //Reemplazo por tamaño del header sin tener en cuenta ultimo padding
    memcpy(&header[12], &real_header_length, 4);
    self->header = malloc(sizeof(unsigned char) * self->header_length);
    memcpy(self->header, header, self->header_length);
}


void dbus_encoder_init(dbus_encoder_t *self, char *line, int message_id) {
    self->header_length = 0;
    self->body_length = 0;
    self->body = NULL;
    self->header = NULL;
    self->line_to_encode = line;
    self->message_id = message_id;
}

void dbus_encoder_destroy(dbus_encoder_t *self) {
    if (self->body != NULL) free(self->body);
    if (self->header != NULL) free(self->header);
} 
//Devuelve el indice de la primer ocurrencia de c en el *string. 
//Devuelve -1 si no se encontro
static int _get_index(char* str, char c) {
    char *e = strchr(str, c);
    if (e == NULL) {
        return -1;
    }
    return (int)(e - str);
}


/* libera un array de strings */
static void free_strv(char *strv[]){
    size_t cad_actual = 0;
    while (strv[cad_actual]){
        free(strv[cad_actual]);
        cad_actual++;
        }
    free(strv);
    }

/*
Devuelve laa cantidad de separadores 'separador' en str
Guarda la longitud de la mayor cadena encontrada en mayor_longitud
*/
static size_t _contar_separadores(const char *str, char separador,
                                   size_t* mayor_longitud) {
    size_t k;
    size_t cantidad = 0;
    size_t longitud_str = strlen(str);
    size_t max_largo = 0;
    size_t max_actual = 0;
    for (k = 0; k < longitud_str; k++){
        if (str[k] == separador){
            cantidad++;
            if (max_actual > max_largo){
                max_largo = max_actual;
                max_actual = 0;
                }
            continue;
            }
        max_actual++;
        }
    if (max_actual > max_largo){
        max_largo = max_actual;
        }   
    if (mayor_longitud){
        *mayor_longitud = max_largo;
        }
    return cantidad;
    }

static char **_split2(const char *str, char sep){
    size_t* longitud_subcadenas = malloc(sizeof(size_t));
    size_t cantidad_separadores = _contar_separadores
                                (str,sep,longitud_subcadenas);
    size_t len_str = strlen(str);
    size_t j;
    char** cadena_separada = NULL;
    cadena_separada = malloc((cantidad_separadores+2)*(sizeof(char*)));
    for (j = 0; j <= cantidad_separadores; j++){
        cadena_separada[j] = malloc(((*longitud_subcadenas)+1)*(sizeof(char)));
        }
    size_t letra_str;
    size_t palabra_actual = 0;
    size_t letra_palabra_actual = 0;
    for (letra_str = 0; letra_str < len_str; letra_str++){
        if (str[letra_str] == sep){
            cadena_separada[palabra_actual][letra_palabra_actual] = '\0';
            palabra_actual++;
            letra_palabra_actual = 0;
            continue;
            }
        cadena_separada[palabra_actual][letra_palabra_actual]
                             = str[letra_str];     
        letra_palabra_actual++;
        }
    cadena_separada[palabra_actual][letra_palabra_actual] = '\0';
    cadena_separada[cantidad_separadores+1] = NULL;
    free(longitud_subcadenas);  
    return cadena_separada; 
    }

bool dbus_encoder_create_send_message(dbus_encoder_t *self) {
    //Almacena los argumentos, (ruta, destino, interfaz, metodo(arg1, ..))
    char **args2 = _split2(self->line_to_encode, ' ');
    int open_parentheses = _count_chars(args2[3], '(');
    int closing_parentheses = _count_chars(args2[3], ')');
    if ((open_parentheses != closing_parentheses) ||
                            (open_parentheses > 1)) return false;
    int params_quantity = 0;
    //int params_quantity = 0;
    if (open_parentheses == 1) { 
        int index_open_parentheses = _get_index(args2[3], '(');
        int index_closing_parentheses = _get_index(args2[3], ')');
        int commas = _count_chars(args2[3], ',');
        if ( (index_closing_parentheses - index_open_parentheses)
             > 1) params_quantity = commas + 1;
    }
    char **method_params = NULL;
    char **method_params1 = NULL;
    char **method_params2 = NULL;
    if (params_quantity >= 1) { 
        method_params1 = _split2(args2[3], '(');
        free(args2[3]);
        args2[3] = method_params1[0];
        method_params1[0] = malloc((sizeof(char))); //Puntero dummy
        method_params2 = _split2(method_params1[1], ')');
        method_params = _split2(method_params2[0], ',');
    }

    if (params_quantity > 0) {
        _calculate_body_length(self, method_params, params_quantity);
        _make_body(self, method_params, params_quantity);
    }

    _calculate_header_size(self, args2, 4, params_quantity);
    _create_header(self, args2, 4, params_quantity);
    free_strv(args2);

    if (method_params1 != NULL) free_strv(method_params1);
    if (method_params2 != NULL) free_strv(method_params2);
    if (method_params != NULL) free_strv(method_params);
    return true;
}

void dbus_decoder_init(dbus_decoder_t *self) {
    self->encoded_message = NULL;
    self->decoded_message = malloc(sizeof(dbus_message_t));
    dbus_message_init(self->decoded_message);
    self->header_length = 0;
    self->body_length = 0;
    self->header_real_length = 0;
    self->method_params_q = 0;
}

//Offset es un int que apunta al primer byte de los 4 del entero.
int _get_current_param_length(dbus_decoder_t *self, int offset,
                             unsigned char param_type) {
    int l;
    if (param_type != 0x09) {
        l = self->encoded_message[offset] +
           (self->encoded_message[offset + 1] << 8) +
           (self->encoded_message[offset + 2] << 16) +
           (self->encoded_message[offset + 3] << 24);
    } else {
        l = self->encoded_message[offset];
    }
    return l;
}

//Offset es la posicion actual del primer byte descriptivo
//length es la longitud del parametro actual
int _get_next_offset(int offset, int length) {
    //Sumo 1 por el /0
    int l = _get_closest_multiply(length + 1, 8);
    return offset + 8 + l;
}

/* offset: Posicion actual del mensaje
   type: tipo de parametro
   l: longitud del parametro
   Guarda en self->mensaje el parametro (ruta, destino, interfaz o metodo)
   Si es de tipo firma, se guarda la cantidad de parametros en self->method_params_q
*/
void _save_current_parameter(dbus_decoder_t *self, int offset,
                             unsigned char type, int l) {
    int description_offset = offset + 8;
    if (type == 0x09) {
        self->method_params_q = l;
    } else if (type == 0x01) {
        //Ruta
        self->decoded_message->ruta = malloc(sizeof(char) * (l + 1)); 
        memcpy(self->decoded_message->ruta, 
            &self->encoded_message[description_offset], l + 1);
    } else if (type == 0x06) {
        //destino
        self->decoded_message->destino = malloc(sizeof(char) * (l + 1)); 
        memcpy(self->decoded_message->destino,
         &self->encoded_message[description_offset], l + 1);
    } else if (type == 0x02) {
        //interfaz
        self->decoded_message->interfaz = malloc(sizeof(char) * (l + 1)); 
        memcpy(self->decoded_message->interfaz,
         &self->encoded_message[description_offset], l + 1);
    } else if (type == 0x03) {
        //metodo
        self->decoded_message->metodo = malloc(sizeof(char) * (l + 1)); 
        memcpy(self->decoded_message->metodo,
         &self->encoded_message[description_offset], l + 1);
    }
}
void _dbus_decoder_decode_header(dbus_decoder_t *self) {
    int offset = 0;
    while (offset < self->header_real_length - 16) {
        unsigned char byte_type = self->encoded_message[offset];
        uint32_t current_length = 
                _get_current_param_length(self, offset + 4, byte_type);
        _save_current_parameter(self, offset, byte_type, current_length);
        offset = _get_next_offset(offset, current_length);
    }
}
// Guarda los parametros del body,
// en orden, en self->decoded_message->parametros
void _dbus_decoder_decode_body(dbus_decoder_t *self) {
    int body_offset = self->header_real_length - 16;
    int body_end = self->header_real_length + self->body_length - 16;
    self->decoded_message->parametros =
         malloc(sizeof(char*) * self->method_params_q);
    self->decoded_message->cantidad_parametros =
         self->method_params_q;
    int current_param = 0;
    while (body_offset < body_end) {
        uint32_t l = _get_current_param_length
                        (self, body_offset, 0x73);
        self->decoded_message->parametros[current_param]
                     = malloc(sizeof(char) * (l + 1));
        int description = body_offset + 4; //Offset al primer byte descriptivo
        memcpy(self->decoded_message->parametros[current_param],
                 &self->encoded_message[description], l + 1);
        //Incremento uno por \0 y 4 por bytes de enteros
        body_offset += l + 5;
        current_param++;
    } 
}

dbus_message_t* dbus_decoder_decode(dbus_decoder_t *self,
                                 unsigned char *message) {
    self->encoded_message = message;
    _dbus_decoder_decode_header(self);
    if (self->method_params_q > 0) {
       _dbus_decoder_decode_body(self);
    }
    return self->decoded_message;
}

ssize_t dbus_decoder_set_descriptions(dbus_decoder_t *self, unsigned char *m) {
    self->body_length = m[4] + (m[5] << 8) + 
                        (m[6] << 16) + (m[7] << 24);
    self->decoded_message->id_mensaje = m[8] + (m[9] << 8)
                     + (m[10] << 16) + (m[11] << 24);
    self->header_length = m[12] + (m[13] << 8)
                         + (m[14] << 16) + (m[15] << 24);
    ssize_t header_real_bytes = _get_closest_multiply(self->header_length, 8);
    self->header_real_length = header_real_bytes;
    ssize_t remaining_bytes = header_real_bytes - 16 + self->body_length;
    return remaining_bytes;
}
