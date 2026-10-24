#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define PUERTO 9999 
#define BUFFER 1024 

int crear_servidor();
void gestionar_clientes(int servidor);
void crear_hijo(int servidor, int cliente);
void enviar_archivo(int cliente);

int crear_servidor() {
	int descriptor;  

	struct sockaddr_in direccion = {0};

	descriptor = socket(AF_INET, SOCK_STREAM, 0); 

	if(descriptor == -1) {
		perror("Error en la creación del socket"); 
		exit(-1); 
	}

	direccion.sin_family = AF_INET; 
	direccion.sin_addr.s_addr = htonl(INADDR_ANY); 
	direccion.sin_port = htons(PUERTO); 

	if(bind(descriptor, (struct sockaddr *) &direccion, sizeof(direccion)) == -1) {
		perror("Error en la instrucción bind"); 
		close(descriptor); 
		exit(-1); 
	}
	return descriptor; 
}

void gestionar_clientes(int servidor) {
	int cliente; 

	signal(SIGCHLD, SIG_IGN); 

	int llamada = listen(servidor, 5); 

	if(llamada == -1) {
		perror("Error en la instrucción listen"); 
		close(servidor);
		exit(-1); 
	} 
	
	printf("Servidor escuchando en el puerto %d\n", PUERTO); 
	while(1) {
		cliente = accept(servidor, NULL, NULL); 

		if(cliente == -1) {
			perror("Error al aceptar la conexión"); 
		} else {
			printf("Cliente conectado\n"); 
			crear_hijo(servidor, cliente); 
		} 
	}
}

void enviar_archivo(int cliente) {
	int archivo; //guarda el descriptor de Google.html
	char buffer[BUFFER]; //almacena temporalmente los datos que se leen
	int bytes_leidos; //bytes leidos por read
	int bytes_enviados; //bytes enviados en una llamada send
	int total_enviados; //cuantos bytes del bloque se han enviado

	archivo = open("Google.html", O_RDONLY); 

	if(archivo == -1)  {
		perror("Error en la apertura de Google.html");
		return; 
	}

	bytes_leidos = read(archivo, buffer, BUFFER); 

	while(bytes_leidos > 0) {
		total_enviados = 0; 

		while(total_enviados < bytes_leidos) {
			bytes_enviados = send(cliente, buffer + total_enviados, bytes_leidos - total_enviados,MSG_NOSIGNAL); 

			if(bytes_enviados <= 0)  {
				perror("Error al enviar el archivo"); 
				close(archivo); 
				return; 
			}
			total_enviados = total_enviados + bytes_enviados; 
		}
		bytes_leidos = read(archivo, buffer, BUFFER); 
	}

	if(bytes_leidos == -1) {
		perror("Error al leer Google.html"); 
	}
	close(archivo); 
}

void crear_hijo(int servidor, int cliente) {
	pid_t pid; 

	pid = fork(); 

	if(pid == -1) {
		perror("Error al crear el proceso hijo"); 
		close(cliente); 
		return; 
	} else if(pid == 0) {
		close(servidor); 
		enviar_archivo(cliente);
		close(cliente);  
		exit(0); 
	} else {
		close(cliente);
	}
}

int main() {

	int servidor = crear_servidor(); 

	gestionar_clientes(servidor);  

	close(servidor); 

	return 0; 
}

