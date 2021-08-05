#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>

#include "helpers.h"

void usage(char*file)
{
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];


	/*Deschidere socket*/

	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) 
		perror("eroare la crearea sock");
	
	/* Deschidere fisier pentru citire */
	DIE((fd=open(argv[3],O_RDONLY))==-1,"open file");
	
	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
		
	to_station.sin_family = AF_INET;
	to_station.sin_port = htons(12345);

	inet_aton("127.0.0.1", &to_station.sin_addr);
	
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	

	memset(buf, 0, BUFLEN);
	int send;
	int result = read(fd, buf, BUFLEN - 1);

	while (result > 0) {
		send = sendto(sock, buf, result, 0, (struct sockaddr*) &to_station, sizeof(to_station));
		if (send < 0)
			perror("eroare la sends");
		
		memset(buf, 0, BUFLEN);
		result = read(fd, buf, BUFLEN - 1);
	}


	/*Inchidere socket*/

	close(sock);

	/*Inchidere fisier*/

	close(fd);

	return 0;
}