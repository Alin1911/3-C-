#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);
}

int main(void)
{
	msg r;
	int res;
	unsigned char mask = 1 << 7;
	int i, result =0;
	mask >>= 1;

	int miner_destinatie;

  	miner_destinatie = open("destinatie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	for (i = 0; i < COUNT; i++) {
		/* wait for message */
		res = recv_message(&r);
		//printf("%s\n",r.payload);

		if (res < 0) {
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		}

		//verificam daca mesajul are paritatea corecta
		for(int j = 0 ; j < strlen ( r.payload); j++){
       		for (int r1 = 0; r1 < 7; r1++) {
    			result ^= (r.payload[j] & mask) != 0;
    			mask >>= 1;
			}
       	}

		if (r.parity != result){ 
            printf("Error detected\n");
        }
        else
        {
        	write(miner_destinatie, r.payload, r.len);
        	memset(&r, 0, sizeof(r));

        }
  

		/* send dummy ACK */
		res = send_message(&r);
		if (res < 0) {
			perror("[RECEIVER] Send ACK error. Exiting.\n");
			return -1;
		}
	}

	printf("[RECEIVER] Finished receiving..\n");


  close(miner_destinatie);
	return 0;
}
