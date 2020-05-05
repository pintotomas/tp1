#include "dbus.h"
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
/* Separa los strings en line y los guarda en el array strings.
   Se utiliza por primera vez el delimitador delim1, y luego delim2 hasta finalizar el recorrido del string.
   si se pasa por parametro delim3, se aplicara antes de iniciar los demas splits
 */
bool _split(char *line, char* strings[], size_t size, char *delim1, char *delim2, char *delim3) {
    
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

int _count_chars(const char* string, char ch)
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

void _calculate_body_length(dbus_encoder_t *self, char* strings[], int params_quantity) {
    int body_total_length = 0;
    for (int i= 1; i < params_quantity; i++) {
        //Por cada parametro necesito 4 bytes p/ header y luego el str y el 00
        body_total_length += 4 + strlen(strings[i]) + 1;
    }    
    self->body_length = body_total_length;
}   

void _make_body(dbus_encoder_t *self, char* strings[], int params_quantity) {



    // int body_total_length = 0;
    // for (int i= 1; i < params_quantity; i++) {
    //     //Por cada parametro necesito 4 bytes p/ header y luego el str y el 00
    //     body_total_length += 4 + strlen(strings[i]) + 1;
    // }   
    //unsigned char body[body_length];
    unsigned char body[self->body_length];

    //unsigned char *s_ptr;
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
    // for (int j = 0; j < body_length; j++) {
    //     printf("Current byte: %x\n", body[j]);   
    //     //printf("Current byte: %x\n", body[j]);    
    // }
    //s_ptr = malloc(sizeof(unsigned char) * self->body_length);
    self->body = malloc(sizeof(unsigned char) * self->body_length);
    //memcpy(s_ptr, body, self->body_length);
    memcpy(self->body, body, self->body_length);


    //return s_ptr;   
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

//Devuelve el numero mas cercano de n que es multiplo de 8 (Mayor o igual a n)
int _get_closest_multiply(int n, int multiply) {
    int x = 0;
    if ((n % multiply) == 0) {
        x = n;
    }
    else {
        x = n + (8 - n % 8);
    }
    return x;
}
void _calculate_header_size(dbus_encoder_t *self, char* params[], int args_quantity, int params_quantity) {
    //16 bytes fijos 
    int header_size = 16; 
    for (int i = 0; i < args_quantity; i++) {
        //4 bytes para la descripcion del parametro + 
        //4 bytes para la longitud del parametro
        header_size += 8; 
        //longitud de la descripcion del param + \0
        int z = strlen(params[i]) + 1;
        //Alineo a 8 bytes
        int x = _get_closest_multiply(z, 8);
        header_size += x;

        // printf("Closest multiple of 8: %d\n", x);
    }
    if (params_quantity > 0) {
        printf("PARAMS QUANTITY: %d\n", params_quantity);
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
    printf("@@@@@@@@@@@Header size@@@@@@@@@@@: %d\n", self->header_length);
    //return header_size;
}

/* Genera un header de largo header_size bytes 
params debe estar en el orden <destino> <path> <interfaz> <metodo>
*/
void _create_header(dbus_encoder_t *self, char* params[], int args_quantity, int params_q) {

    printf("@@@@@@@@@@@BODY SIZE@@@@@@@@@@@: %d\n", self->body_length);
    //unsigned char *s_ptr;

    unsigned char header[self->header_length];
    uint32_t body_length32 = ((uint32_t) self->body_length);
    unsigned char h1[16] = {0x6c, 0x01, 0x00, 0x01};
    //body_size_32 = htonl(body_size_32); --para despues
    int header_position = 0;
    memcpy(&header[header_position], &h1, 4);
    header_position += 4;
    memcpy(&header[header_position], &body_length32, 4);
    header_position += 4;
    unsigned char message_id[16] = {0x02, 0x00, 0x00, 0x00};
    memcpy(&header[header_position], &message_id, 4);
    header_position +=4;
    //Posicion del header size: 4+4+4: 12
    unsigned char dummy_header_length[16] = {0xff, 0xff, 0xff, 0xff};
    memcpy(&header[header_position], &dummy_header_length, 4);
    header_position +=4;
    //array con las descripciones de cada parametro a enviar.
    unsigned char array[4][4] =
{
    { 0x01, 0x01, 0x6f, 0x00 },
    { 0x06, 0x01, 0x73, 0x00 },
    { 0x02, 0x01, 0x73, 0x00 },
    { 0x03, 0x01, 0x73, 0x00 }
};
    int last_padding;
    for (int u = 0; u < args_quantity; u++) {
        last_padding = _get_closest_multiply(strlen(params[u]) + 1, 8) - strlen(params[u]);
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


        //printf("For the string: %s with length: %ld we have to add a padding of: %d\n", params[u], strlen(params[u]), last_padding);
    }

    if (params_q > 0) {
        // 4 bytes de header, 1 p/cant de params 1 's' por cada
        // param, y un /0 al final (Por eso el +6). Completa con /0 para alinear.
        unsigned char h_params[16] = {0x08, 0x01, 0x67, 0x00};
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

    for (int j = 0; j < header_position; j++) {
        printf("Current byte: %x (char: %c)\n", header[j], header[j]);

        //printf("Current byte: %x\n", body[j]);    
    }
    printf("@@@@@@@@@@@LAST PADDING·@@@@@@@@@@: %d\n", last_padding);

    //s_ptr = malloc(sizeof(unsigned char) * self->header_length);
    self->header = malloc(sizeof(unsigned char) * self->header_length);
    //memcpy(s_ptr, header, self->header_length);
    memcpy(self->header, header, self->header_length);


//    return s_ptr;   

    // for (int i = 0; i < 4; i++) {
    //     // printf("param: %s\n",params[i]);
    //     // printf("Strlen: %ld\n",strlen(params[i]));
    //     // printf("Strlen + /0: %ld\n",strlen(params[i]) + 1);
    //     int z = strlen(params[i]) + 1;
    //     int x;
    //     if ((z % 8) == 0) {
    //         x = z;
    //     }
    //     else {
    //         x = z + (8 - z % 8);
    //     }
    //     x = x;
    //     //printf("Closest multiple of 8: %d\n", x);
    // }
}


void dbus_encoder_init(dbus_encoder_t *self, char *line) {

    self->header_length = 0;
    self->body_length = 0;
    self->body = NULL;
    self->header = NULL;
    self->line_to_encode = line;

}

void dbus_encoder_destroy (dbus_encoder_t *self) {
    if (self->body != NULL) free(self->body);
    if (self->header != NULL) free(self->header);
} 

bool dbus_encoder_create_send_message(dbus_encoder_t *self) {

    //Almacena los argumentos, (ruta, destino, interfaz, metodo(arg1, ..))
    char *args[4] = {0}; 
    //unsigned char *body = NULL;
    //int body_length_calc = 0;
    //if (!_split(line, args, 4, " ", " ", NULL)) return false;
    if (!_split(self->line_to_encode, args, 4, " ", " ", NULL)) return false;
    int open_parentheses = _count_chars(args[3], '(');
    int closing_parentheses = _count_chars(args[3], ')');
    if ((open_parentheses != closing_parentheses) || (open_parentheses > 1)) return false;
    int params_quantity = 1;
    if (open_parentheses == 1) params_quantity = _count_chars(args[3], ',') + 1 + params_quantity;
    char *method[params_quantity];
    if (!_split(args[3], method, params_quantity, "(", ",", ")"));
    if (params_quantity > 1) {
        //body_length_calc = _calculate_body_length(method, params_quantity);
        //self->body_length = _calculate_body_length(self, method, params_quantity);
        _calculate_body_length(self, method, params_quantity);

        //body = _make_body(method, params_quantity, body_length_calc);
        //self->body = _make_body(self, method, params_quantity, self->body_length);
        //self->body = _make_body(self, method, params_quantity);
        _make_body(self, method, params_quantity);


    }

    //*body_length = body_length_calc;
    //int header_size = _calculate_header_size(args, 4, params_quantity - 1);
    //self->header_length = _calculate_header_size(args, 4, params_quantity - 1);
    _calculate_header_size(self, args, 4, params_quantity - 1);

    printf("@@@@@@@@@@@Header size@@@@@@@@@@@: %d\n", self->header_length);
    //_create_header(args, 4, header_size, body_length_calc, params_quantity - 1);
    //self->header = _create_header(args, 4, self->header_length, self->body_length, params_quantity - 1);
    //_create_header(self, args, 4, self->header_length, self->body_length, params_quantity - 1);
    _create_header(self, args, 4, params_quantity - 1);
    
    return true;


    // for (int j = 0; j < body_length_calc; j++) {
    //     printf("Current byte: %x\n", body[j]);   
    //     //printf("Current byte: %x\n", body[j]);    
    // } 
    // printf("ruta: %s\n", args[0]);
    // printf("destino: %s\n", args[1]);
    // printf("interfaz: %s\n", args[2]);

    //return body;
}

void dbus_decoder_init(dbus_decoder_t *self) {
    self->encoded_message = NULL;
    self->header_length = 0;
    self->body_length = 0;
}

void dbus_decoder_decode(dbus_decoder_t *self, unsigned char *message) {
    printf("@@@@@@@Received 120 bytes@@@@@@\n");

    self->encoded_message = message;
    for (int j = 0; j < self->header_length + self->body_length - 16; j++) {
        printf("Current byte: %x, (Char: %c) \n", message[j], message[j]);   
        //printf("Current byte: %x\n", body[j]);    
    } 
    return;
}

ssize_t dbus_decoder_set_descriptions(dbus_decoder_t *self, unsigned char *m) {

    //Little endian
    // printf("@@@@@@@@@@qReceived: %d bytes@@@@@@@q\n", 16);

    // for (int j = 0; j < 16; j++) {
    //     printf("Current byte received: %x, (char): %c\n", m[j], m[j]);   
    //     //printf("Current byte: %x\n", body[j]);    
    // } 
    self->body_length = m[4] + (m[5] << 8) + (m[6] << 16) + (m[7] << 24);
    self->header_length = m[12] + (m[13] << 8) + (m[14] << 16) + (m[15] << 24);

    //Big endian
    //self->header_length = (m[4]  << 24) + (m[5] << 16) + (m[6] << 8) + m[7];
    //self->body_length = (m[12] << 24) + (m[13] << 16) + (m[14] << 8) + m[15];
    ssize_t remaining_bytes = _get_closest_multiply(self->header_length 
        - 16 + self->body_length, 8);
    //printf("header len: %d\n", self->header_length);   
    // printf("body len: %d\n", self->body_length);   
    //printf("remaining byetes: %ld\n", remaining_bytes);   

    return remaining_bytes;
}
