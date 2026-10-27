#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999
#define BUFFER 1024 

int crear_cliente() {
	int cliente; 

	cliente = socket(AF_INET, SOCK_STREAM, 0);

	if(cliente == -1) {
		perror("Error al crear el socket del cliente");
		exit(-1);  
	} 
	return cliente; 
}

void conectar_servidor(int cliente, char *ip) {
	struct sockaddr_in direccion = {0};

	direccion.sin_family = AF_INET; 
	direccion.sin_port = htons(PUERTO); 

	int resultado = inet_pton(AF_INET, ip, &direccion.sin_addr);

    if(resultado != 1) {
        printf("Error: direccion IP no válida\n");
        close(cliente);
        exit(-1);
    }

	int conexion = connect(cliente, (struct sockaddr *)&direccion, sizeof(direccion));

	if(conexion == -1) {
		perror("Error en la conexión con el servidor"); 
		close(cliente); 
		exit(-1); 
	}
}

void recibir_archivo(int cliente) {
	char buffer[BUFFER]; 
	int bytes_recibidos; 
	int total_escritos; 
	int bytes_escritos;
	int error = 0; 

	bytes_recibidos = recv(cliente, buffer, BUFFER, 0); 

	while(bytes_recibidos > 0 && error == 0) {
		total_escritos = 0; 

		while(total_escritos < bytes_recibidos && error == 0) {
			bytes_escritos = write(1, buffer + total_escritos, bytes_recibidos - total_escritos); 
			
			if(bytes_escritos <= 0) {
				perror("Error al mostrar el archivo"); 
				error = 1; 
			} else {
				total_escritos = total_escritos + bytes_escritos;  
			}
		}
		if(error == 0) {
			bytes_recibidos = recv(cliente, buffer, BUFFER, 0); 
		}
	}

	if(bytes_recibidos == -1) {
		perror("Error al recibir el archivo"); 
	}
}

int main(int argc, char *argv[]) {

	if(argc != 2)  {
		printf("Número de parámetros incorrecto\n"); 
		printf("USO: ./cliente IP_servidor\n");
		exit(-1); 
	}

	int cliente = crear_cliente(); 

	conectar_servidor(cliente, argv[1]);

	recibir_archivo(cliente); 

	close(cliente); 

	return 0; 
}