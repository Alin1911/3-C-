#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    char *message, *string;
    char *response;
    int sockfd, s;
    char *host = "34.118.48.238";
    char *datatype = "application/json";

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;

    char **cookies = calloc(1,sizeof(char*));
    cookies[0] = calloc(BUFLEN,sizeof(char));

    char **autorizare = calloc(1,sizeof(char*));
    autorizare[0] = calloc(BUFLEN,sizeof(char));

    char comanda[30], username[50], password[50], ID[50];
 	char title[30], author[30], genre[20], publisher[50], page_count[15]; 

    do{
    	s = 99;
    	scanf("%s",comanda);

    	if(strcmp(comanda,"register") == 0)
    		s = 0;

    	if(strcmp(comanda,"exit") == 0)
    		s = 1;

    	if (strcmp(comanda, "login") == 0)
			s = 2;

		if (strcmp(comanda, "enter_library") == 0)
			s = 3;

		if (strcmp(comanda, "get_books") == 0)
			s = 4;

		if (strcmp(comanda, "get_book") == 0)
			s = 5;

		if (strcmp(comanda, "add_book") == 0)
			s = 6;

		if (strcmp(comanda, "delete_book") == 0)
			s = 7;

		if (strcmp(comanda, "logout") == 0)
			s = 8;

    	switch (s){
    		//register
    		case 0:{
    			sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    			//citesc si adaug in obiectul json detele utilizatorului

    			printf("username=");
				scanf("%s", username);
				json_object_set_string(root_object, "username", username);

    			printf("password=");
				scanf("%s", password);
				json_object_set_string(root_object, "password", password);

				serialized_string = json_serialize_to_string_pretty(root_value);

    			//compun mesajul pentru POST
    			//POST
    			printf("\n\n=====Mesajul de POST pentru inregistrarea unui cont=====\n\n");
    			message = compute_post_request(host, "/api/v1/tema/auth/register", datatype, &serialized_string , 1, NULL, 0,NULL,0);
    			printf("%s\n", message);

    			//trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n", response);

    			close_connection(sockfd);
    			s = 99;
    			break;
    		}
    		//login
    		case 2: {
    			sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    			///citesc si adaug in obiectul json detele utilizatorului

    			printf("username=");
				scanf("%s", username);
				json_object_set_string(root_object, "username", username);

    			printf("password=");
				scanf("%s", password);
				json_object_set_string(root_object, "password", password);

				serialized_string = json_serialize_to_string_pretty(root_value);


    			//POST
    			printf("\n\n=====Mesajul de POST pentru conectarea la un cont=====\n\n");
    			message = compute_post_request(host, "/api/v1/tema/auth/login", datatype, &serialized_string , 1, NULL, 0,NULL,0);
    			printf("%s\n", message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n", response);

    			//construiest cookiurile utilizatorului
    			if(strstr(response,"HTTP/1.1 200 OK") != NULL ){
    				string = strstr(response,"Set-Cookie: ");
    				cookies[0] = strtok (string,"; \n");
  					cookies[0] = strtok (NULL, ";\n ");
  				}
    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//enter_library
			case 3: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

				//GET
  				printf("\n\n=====Mesajul de GET pentru o cerere de access in librarie=====\n\n");
    			message = compute_get_request(host, "/api/v1/tema/library/access", NULL, cookies, 1, NULL, 0);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);

    			//construiest accestul  utilizatorului in librarie folosind tokenul JWT
    			if(strstr(response,"HTTP/1.1 200 OK") != NULL ){
    				string = strstr(response,"\":\"");
    				autorizare[0] = strtok (string,"\"");
  					autorizare[0]  = strtok (NULL, "\"");
  				}

    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//get_books
			case 4: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

				//GET
  				printf("\n\n=====Mesajul de GET pentru cererea listei cu cartile din librarie=====\n\n");
    			message = compute_get_request(host, "/api/v1/tema/library/books", NULL, NULL, 0, autorizare, 1);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);

    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//get_book -id
			case 5: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
				
				printf("id=");
				scanf("%s", ID);

				//construiesc calea catre cartea cu ID-ul ID
				char string2[] = "/api/v1/tema/library/books/";
				strcat(string2,ID);

				//GET
  				printf("\n\n=====Mesajul de GET pentru detaliile unei carti=====\n\n");
    			message = compute_get_request(host, string2, NULL, NULL, 0, autorizare, 1);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);


    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//add_book
			case 6: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  				//citesc si adaug in obiectul json detele cartii pe care doresc sa o adaug
  				printf("title=");
				scanf("%s", title);
				json_object_set_string(root_object, "title", title);

    			printf("author=");
				scanf("%s", author);
				json_object_set_string(root_object, "author", author);

				printf("genre=");
				scanf("%s", genre);
				json_object_set_string(root_object, "genre", genre);

    			printf("publisher=");
				scanf("%s", publisher);
				json_object_set_string(root_object, "publisher", publisher);

				printf("page_count=");
				scanf("%s", page_count);
				json_object_set_string(root_object, "page_count", page_count);

				serialized_string = json_serialize_to_string_pretty(root_value);

				//POST
  				printf("\n\n=====Mesajul de POST pentrua adaugarea unei carti=====\n\n");
    			message = compute_post_request(host, "/api/v1/tema/library/books", datatype, &serialized_string , 1, NULL, 0,autorizare,1);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);


    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//delete_book -id
			case 7: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

				printf("id=");
				scanf("%s", ID);

				//construiesc calea catre cartea cu ID-ul ID
				char string2[] = "/api/v1/tema/library/books/";
				strcat(string2,ID);

				//POST
  				printf("\n\n=====Mesajul de DELETE pentru stergerea unei carti=====\n\n");
    			message = compute_delete_request(host, string2, NULL, NULL, 0, autorizare, 1);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
    			printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);

    			close_connection(sockfd);
    			s = 99;
    			break;
			}
			//logout
			case 8: {
				sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

				//POST
  				printf("\n\n=====Mesajul de GET pentru delogarea de la server=====\n\n");
    			message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL, 0);
    			printf("%s\n",message);

    			///trimite catre server si preiau raspunsul
    			send_to_server(sockfd, message);
				printf("\n\n=====RASPUNSUL SERVARULUI=====\n\n");
    			response = receive_from_server(sockfd);
    			printf("%s\n",response);

    			//cand utilizatorul se delogheaza sterg datele lui de securitate
    			memset(cookies[0], 0, BUFLEN);
    			memset(autorizare[0], 0, BUFLEN);

    			close_connection(sockfd);
    			s = 99;
    			break; 
			}
    	}

    }while(s != 1);
    return 0;
}
