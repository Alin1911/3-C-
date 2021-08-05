#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);}

int main(int argc, char *argv[])
{
	msg t;
	int res;
	int w = (atoi (argv[1]) * 1000) / (sizeof (msg) * 8);
	int miner_sursa;
	//citire // pentru verificat maxim w cadre// total cadre//
	int confirmari[3] = {0};
	unsigned char mask = 1 << 7;
	int i, result =0;
	mask >>= 1;


	printf("[SENDER] Starting.\n");
	init(HOST, PORT);

	miner_sursa = open("sursa.txt", O_RDONLY);

    if (miner_sursa < 0)
        fatal("Nu pot deschide un fisier");
    else
		memset(&t, 0, sizeof(t));

	lseek(miner_sursa, 0, SEEK_SET);

	//trimitem w cadre pentru a popula canalul
	for (  i = 0; i < w; i++)
	{
		
		strcpy(t.payload,"asaasdasdasdfnudiioqnojdiqowmdpmkewcmoipjcomweoimwle,");
		t.len =strlen(t.payload);
		for(int j = 0 ; j < strlen ( t.payload); j++){
       		for (int k = 0; k < 7; k++) {
    			result ^= (t.payload[j] & mask) != 0;
    			mask >>= 1;
			}
       	}
       	t.parity = result;
       	res = send_message(&t);
       	if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}

	}

	confirmari[1]=w;

	//cat timp nu s-au trimis COUNT cadre 
	while (confirmari[2] < COUNT) {
		memset(&t, 0, sizeof(t));

        // aici se va intoarce pentru cazul in care am citit din fisier dar nu s-a trimis
		nuAtrimis:

		if (confirmari[1] < w){

			confirmari[0] = read(miner_sursa, t.payload, MSGSIZE -1 );
			//eroare la citire
		if (confirmari[0] < 0)
            fatal("Eroare la citire");

  			//cazul in care se poate trimite
       		t.len = strlen(t.payload);
       		t.parity = 0;

       		for(int j = 0 ; j < strlen ( t.payload); j++){
       			for (i = 0; i < 7; i++) {
    				result ^= (t.payload[j] & mask) != 0;
    				mask >>= 1;
				}
       		}

       		t.parity = result;
 			//printf("%s\n",t.payload);
			res = send_message(&t);
  
			//max w cadre
			confirmari[1]++;
			//total cadre
			confirmari[2]++;

			if (res < 0) {
				perror("[SENDER] Send error. Exiting.\n");
				return -1;
			}
			memset(&t, 0, sizeof(t));
		}
		else{
			// cazul in care nu se poate trimite si asteptam
			res = recv_message(&t);
			if (res < 0) {
				perror("[SENDER] Receive error. Exiting.\n");
				return -1;
			}else

			confirmari[1]--;
			goto nuAtrimis;
		}
    }

	printf("[SENDER] Job done, all sent.\n");
	close(miner_sursa);
	return 0;
}
