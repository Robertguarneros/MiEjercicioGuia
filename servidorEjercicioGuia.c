#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include<stdbool.h>  
#include <pthread.h>

//miejercicio
void* AtenderCliente(void* socket)
{
	int sock_conn;
	int* s;
	s = (int*)socket;
	sock_conn = *s;

	//int socket_conn = * (int *) socket;

	char peticion[512];
	char respuesta[512];
	int ret;


	int terminar = 0;
	while (terminar == 0)
	{

		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibida una petición\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret] = '\0';
		char* p = strtok(peticion, "/");
		int codigo = atoi(p);
		char nombre[20];

		//Escribimos la peticion en la consola
		if (codigo != 0) {
			printf("La petición es: %s\n", peticion);
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}

		if (codigo == 0)
			terminar = 1;

		else if (codigo == 1) //piden la longitd del nombre
			sprintf(respuesta, "%d", strlen(nombre));
		else if (codigo == 2)
		{
			// quieren saber si el nombre es bonito
			if ((nombre[0] == 'M') || (nombre[0] == 'S'))
				strcpy(respuesta, "SI");
			else
				strcpy(respuesta, "NO");
		}
		else if (codigo == 3)//decir si es alto 
		{
			p = strtok(NULL, "/");
			float altura = atof(p);
			if (altura > 1.70)
				sprintf(respuesta, "%s: eres alto", nombre);
			else
				sprintf(respuesta, "%s: no eres alto", nombre);
		}
		else if (codigo == 4)//palindromo
		{
			bool palindromo = false;
			int longitud_nombre = strlen(nombre);
			for (int k = 0; k < longitud_nombre; k++) {
				if (nombre[k] == nombre[longitud_nombre - k - 1]) {
					palindromo = true;
				}
			}
			sprintf("Tu nombre, %s, si es un palindromo", nombre);

		}
		else if (codigo == 5)//nombre en maysuculas
		{
			for (int j = 0; nombre[j] != '\0'; ++j) {
				nombre[j] = toupper(nombre[j]);
			}
			sprintf(respuesta, "%s", nombre);
		}

		if (codigo != 0) {
			// Enviamos la respuesta
			write(sock_conn, respuesta, strlen(respuesta));
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);

}


//miejercicio
int main(int argc, char* argv[])
{

	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;

	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port


	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;

	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind");

	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");

	int i;
	int sockets[100];
	pthread_t thread;
	i = 0;
	// Bucle para atender a 5 clientes
	for (;;) {
		printf("Escuchando\n");

		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");

		sockets[i] = sock_conn;
		//sock_conn es el socket que usaremos para este cliente

		// Crear thead y decirle lo que tiene que hacer

		pthread_create(&thread, NULL, AtenderCliente, &sockets[i]);
		i = i + 1;

	}
}
