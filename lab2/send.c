#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);
}

int main(int argc,char** argv){

  	init(HOST,PORT);
  	msg t;
	int miner_sursa;
    int copiat = 0;

    printf("[send] Sending dummy...\n");
    miner_sursa = open("sursa.txt", O_RDONLY);

    if (miner_sursa < 0)
        fatal("Nu pot deschide un fisier");

    for (int i = 0; i < MAX_LEN; ++i)
  		t.payload[i] = 0;


    lseek(miner_sursa, 0, SEEK_SET);

    
    while ((copiat = read(miner_sursa, t.payload, MAX_LEN -1 ))) {

    	if (copiat < 0)
            fatal("Eroare la citire");
  
        t.len = strlen(t.payload);
      	send_message(&t);

   		if (recv_message(&t)<0)
			perror("Receive error ...");
		printf("[send] Got reply with payload: %s\n", t.payload);

		for (int i = 0; i < MAX_LEN; ++i)
			t.payload[i] = 0;
      	
    }
 
  close(miner_sursa);

  return 0;
}
