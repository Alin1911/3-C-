#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);
}

int main(int argc,char** argv){
  msg r;
  int nr = 0;
  init(HOST,PORT);
  int miner_destinatie;

  miner_destinatie = open("destinatie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);


  while(strlen(r.payload) != 0 || nr == 0){

    if (recv_message(&r)<0){
      perror("Receive message");
      return -1;
    }
    printf("[recv] Got msg with payload: <%s>, sending ACK...\n", r.payload);
    write(miner_destinatie, r.payload, strlen(r.payload));
    sprintf(r.payload,"%s", "ACK");
    r.len = strlen(r.payload) + 1;
    send_message(&r);
    nr++;
    printf("[recv] ACK sent\n");
  }
  close(miner_destinatie);
  return 0;
}
