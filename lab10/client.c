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

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char *host = "34.118.48.238";
    char *Weather_API = "api.openweathermap.org";
    char *tipdate = "application/x-www-form-urlencoded";

    sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);       

    printf("\n===================================================================\n");
    // Ex 1.1: GET dummy from main server
    printf("%s\n", "Ex 1.1: GET dummy from main server");
    message = compute_get_request(host, "/api/v1/dummy", NULL, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);


    printf("\n===================================================================\n");
    // Ex 1.2: POST dummy and print response from main server
    printf("%s\n", "Ex 1.2: POST dummy and print response from main server");
    message = compute_post_request(host, "/api/v1/dummy", tipdate, NULL, 0, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);


    printf("\n===================================================================\n");
    // Ex 2: Login into main server
    char **login = calloc(2 , sizeof(char *));
    login[0] = calloc(BUFLEN, sizeof(char));
    strcpy(login[0], "username=student");
    login[1] = calloc(BUFLEN, sizeof(char));
    strcpy(login[1], "password=student");

    printf("%s\n", "Ex 2: Login into main server");
    message = compute_post_request(host, "/api/v1/auth/login", tipdate, login, 2, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);


	printf("\n===================================================================\n");
    // Ex 3: GET weather key from main server
    char **cookies = calloc(1,sizeof(char*));
    cookies[0] = calloc(BUFLEN,sizeof(char));
    strcpy(cookies[0],"connect.sid=s%3AGfMsFHGcrX_fEdvarRWuVATDv1rtWwqF.pY%2Bgotj31F1OqO3fSeUY1qrE9F3lZpat24Q0RKwHNvY");

    printf("%s\n","Ex 3: GET weather key from main server");
    message = compute_get_request(host, "/api/v1/weather/key", NULL, cookies, 1);
    printf("%s\n",message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n",response);


	printf("\n===================================================================\n");
    // Ex 4: GET weather data from OpenWeather API
    char *key = "lat=24&lon=24&appid=b912dd495585fbf756dc6d8f415a7649";
    int sockfdWeather = open_connection("37.139.1.159", 80, AF_INET, SOCK_STREAM, 0);

    printf("%s\n", "Ex 4: GET weather data from OpenWeather API");
    message = compute_get_request(Weather_API,"/data/2.5/weather", key, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfdWeather, message);
    response = receive_from_server(sockfdWeather);
    printf("%s\n", basic_extract_json_response(response));


    printf("\n===================================================================\n");
    // Ex 5: POST weather data for verification to main server
    printf("%s\n", "Ex 5: POST weather data for verification to main server");
    message = compute_post_request(host, "/api/v1/weather/24/24 ", "application/json", NULL, 0, NULL ,0 );
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", basic_extract_json_response(response));
    

    printf("\n===================================================================\n");
    // BONUS: make the main server return "Already logged in!"
    printf("%s\n", "BONUS: make the main server return Already logged in!");
    if (cookies[0] != NULL && strlen(cookies[0]) > 0) {
        printf("%s\n","Already logged in!"); 
    } else {
        message = compute_post_request(host, "/api/v1/auth/login", tipdate, login, 2, NULL, 0);
        printf("%s\n",message);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        printf("%s\n",response);
    }


    printf("\n===================================================================\n");
    // Ex 6: Logout from main server
    printf("%s\n", "Ex 6: Logout from main server");
    message = compute_get_request(host, "/api/v1/auth/logout", NULL, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response); 


    printf("\n===================================================================\n");
    // free the allocated data at the end!

    close_connection(sockfdWeather);
    close_connection(sockfd);
    free(message);
    free(response);
    free(login[0]);
    free(login[1]);
    free(login);
    free(cookies[0]);
    free(cookies);

    return 0;
}
