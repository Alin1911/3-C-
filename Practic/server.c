#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"

typedef struct client{
	int sok;
	int len;
	struct client *next;
}client;

typedef struct resurse{
	char *data;
	char *nume;
	struct resurse *next;
}resurse;
//alocarea unei resurse
resurse *make_resurse(char *nume,char *data){
    resurse *nou = (resurse *)malloc(sizeof(resurse));
    nou->nume = (char *)malloc(sizeof(char) * sizeof(nume));
    nou->data = (char *)malloc(sizeof(char) * sizeof(data));
    strcpy(nou->nume, nume);
    strcpy(nou->data, data);
    return nou;
}
//adaugarea unei resurse in lisata sau crearea listei cu resurse
resurse *resurseL(resurse *lista,char *nume,char *data){
	if(!lista){
        lista = make_resurse(nume,data);
    } else {
        resurse *current = lista;
        while(current->next){
        	current = current->next;
        }
        current->next = make_resurse(nume,data);
        current->next->next = NULL;
    }
	return lista;
}
//functie pentruc autarea unei resurse pe baza numelui
resurse *cauta_resursa(resurse *list, char *nume)
{
	resurse *current = list;
	while(current){
		if(strcmp(current->nume,nume) == 0){
			return current;
		}
		current = current->next;
	}
	return NULL;
}
//functie pentru cautarea unui client pe baza socketului
client *cauta_client(client *list, int sok)
{
	client *current = list;
	while(current){
		if(current->sok == sok){
			return current;
		}
		current = current->next;
	}
	return NULL;
}
//stergerea unei resurse
resurse *delte_res(resurse *list,char *nume){

	resurse *current = list;
	resurse *prv;
	if(!list || strcmp(current->nume,nume) == 0)
		return NULL;
	while(current){
		if(strcmp(current->nume,nume) == 0){
			break;
		}
		prv = current;
		current = current->next;
	}
	prv->next = current->next; 
	free(current);
	return list;
}

//alocarea unui client
client *make_client(int sok){
    client *nou = (client *)malloc(sizeof(client));
	nou->sok = sok;
    return nou;
}
//functie petru adaugarea unui client(crearea listei de clienti)
client *connectC(client *lista,int sok){
	if(!lista){
        lista = make_client(sok);
    } else {
        client *current = lista;
        while(current->next){
        	current = current->next;
        }
        current->next = make_client(sok);
        current->next->next = NULL;
    }
	return lista;
}

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, ret;
	socklen_t clilen;
	client *clientii=NULL;
	resurse *resurseList=NULL;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");

	ret = listen(sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(sockfd, &read_fds);
	fdmax = sockfd;

	while (1) {
		tmp_fds = read_fds; 
		
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == sockfd) {
					// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					DIE(newsockfd < 0, "accept");

					// se adauga noul socket intors de accept() la multimea descriptorilor de citire
					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax) { 
						fdmax = newsockfd;
					}
					clientii = connectC(clientii, newsockfd);

					printf("Noua conexiune de la %s, port %d, socket client %d\n",
							inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);

				}
				else if(i == STDIN_FILENO){
					memset(buffer, 0, BUFLEN);
					if(fgets(buffer, BUFLEN - 1, stdin)){
						if(strcmp(buffer, "quit\n") == 0){
							for(client *temp = clientii; temp; temp = temp->next){
								send(temp->sok, "quit",4, 0);
							}
							close(sockfd);
							return 0;
						}
						if(strcmp(buffer, "status\n") == 0){
							int nr = 0;
							for(client *temp = clientii; temp; temp = temp->next)
								nr++;
							printf("Nr clienti= %d   Lista de resurse =",nr);
							if(!resurseList)
								printf("[]\n");
							else
								for(resurse *temp1 = resurseList; temp1; temp1 = temp1->next)
									printf("%s  ",temp1->nume);
							printf("\n");
						}
						memset(buffer, 0, BUFLEN);
					}
				} else {
					// s-au primit date pe unul din socketii de client,
					// asa ca serverul trebuie sa le receptioneze
					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, sizeof(buffer), 0);
					DIE(n < 0, "recv");


					if (n == 0) {
						// conexiunea s-a inchis
						printf("Socket-ul client %d a inchis conexiunea\n", i);
						close(i);
						
						// se scoate din multimea de citire socketul inchis 
						FD_CLR(i, &read_fds);
					} else {
						if(strncmp(buffer, "put", 3) == 0){
 
							char *data = strtok(buffer, " ");
							char *nume = strtok(NULL, " ");
							data = strtok(NULL, " ");
							resurseList = resurseL(resurseList, nume,data);
							char mes[BUFLEN];
							strcpy(mes,"HTTP/1.1200 OK\r\n\r\n");
							send(i,mes,strlen(mes),0);
						}
						else if(strncmp(buffer, "get", 3) == 0){
							char *nume = strtok(buffer, " \n\t");
							nume = strtok(NULL, " \n\t");
							resurse *aici = cauta_resursa(resurseList, nume);
							if(!aici)
								send(i,"HTTP/1.1 404 NotFound\r\n\r\n",25,0);
							else{
								char mes[BUFLEN];
								strcpy(mes,"HTTP/1.1200 OK\r\n\r\n");
								strcat(mes,"  Nume resursa =  ");
								strcat(mes,aici->nume);
								strcat(mes,"  Date resursa =  ");
								strcat(mes,aici->data);
								send(i,mes,strlen(mes),0);
							}
						}
						else if(strncmp(buffer, "login", 5) == 0){
							char *nume = strtok(buffer, " \n\r\t");
							nume = strtok(NULL, " \n\r\t");
							char *parola = strtok(buffer, " \n\r\t");

						}
						else if(strncmp(buffer, "delete", 6) == 0){
							char *nume = strtok(buffer, " \n\t");
							nume = strtok(NULL, " \n\t");
							resurseList = delte_res(resurseList, nume);
							char mes[BUFLEN];
							strcpy(mes,"HTTP/1.1200 OK\r\n\r\n");
							send(i,mes,strlen(mes),0);
						}
							
					}
					memset(buffer, 0, BUFLEN);   
				}
			}
		}
	}

	close(sockfd);

	return 0;
}
