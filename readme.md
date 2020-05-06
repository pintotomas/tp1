Padron: 98757

Repositorio: https://github.com/pintotomas/tp1

El problema se trata de replicar el protocolo DBUS utilizando sockets TCP/IP en vez de el que utiliza originalmente, UNIX. Para ello se codifico en C un cliente y un servidor que se comunican a traves de sockets, y para realizar la comunicacion, lo hacen mediante la clase protocolo, que codifica y decodifica los mensajes (utilizando las clases dbus decoder y dbus encoder). Para codificarla lee del stdin/archivo que reciba por parametro el cliente y finalmente devuelve un mensaje decodificado al servidor. Este le responde que el mensaje llego Ok.
