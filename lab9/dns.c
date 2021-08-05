// Protocoale de comunicatii
// Laborator 9 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;

	// TODO: set hints
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE | AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;


	// TODO: get addresses
	ret = getaddrinfo(name, NULL, &hints, &result);

	if (ret != 0) {
		printf("%s", gai_strerror(ret));
	}



	// TODO: iterate through addresses and print them

	for (p = result; p != NULL; p = p -> ai_next) {
		if (p -> ai_family == AF_INET) {
			char ip4[INET_ADDRSTRLEN];
			struct sockaddr_in* so = (struct sockaddr_in*) (p -> ai_addr);
			inet_ntop(p -> ai_family, &so -> sin_addr, ip4, sizeof(ip4));
			printf("%s | %d | %s |%d \n", ip4, ntohs(so ->sin_port), p -> ai_canonname, p -> ai_socktype);
		} else if (p -> ai_family == AF_INET6) {
			char ip6[INET6_ADDRSTRLEN];
			struct sockaddr_in6* so6 = (struct sockaddr_in6*) (p -> ai_addr);
			inet_ntop(p -> ai_family, &so6 -> sin6_addr, ip6, sizeof(ip6));
			printf("%s | %d | %s |%d \n", ip6, ntohs(so6 ->sin6_port), p -> ai_canonname, p -> ai_socktype);
			}
	}


	// TODO: free allocated data
	freeaddrinfo(result);

}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	char host[1024];
	char service[20];

	// TODO: fill in address data
	inet_aton(ip, &addr.sin_addr);
	addr.sin_family = AF_INET;

	// TODO: get name and service

	ret = getnameinfo((struct sockaddr*) &addr, sizeof(addr), host, INET_ADDRSTRLEN, service, INET_ADDRSTRLEN, 0);

	if (ret != 0) {
		printf("%s", gai_strerror(ret));
	}
	// TODO: print name and service
		printf("%s |  %s\n",host, service);
}


int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
