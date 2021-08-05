    #include <unistd.h> /* pentru open(), exit() */
    #include <fcntl.h> /* O_RDWR */
    #include <errno.h> /* perror() */
     
    void fatal(char * mesaj_eroare)
    {
        perror(mesaj_eroare);
        exit(0);
    }
     
    int main(void)
    {
        int miner_sursa;
        int copiat, nr = 1 ;
        char buf[1024];
        int v[100];
     
        miner_sursa = open("sursa.txt", O_RDONLY);
     
        if (miner_sursa < 0)
            fatal("Nu pot deschide un fisier");
     
        lseek(miner_sursa, 0, SEEK_SET);

     
        while ((copiat = read(miner_sursa, buf, sizeof(buf)))) {
            if (copiat < 0)
                fatal("Eroare la citire");
        }

        int size = strlen(buf);
        for (int i = 0; i < size; ++i)
        {
            if( buf[i] == '\n'){
                v[nr] = i+1;
                nr++;
                }
        }
        nr--;
        for (int i = nr; i >= 0; i--)
        {
            if(i == nr){
                lseek(miner_sursa, v[i], SEEK_SET);
                copiat = read(miner_sursa, buf, size - v[i]);
                copiat = write(STDOUT_FILENO,buf,copiat);
                write(STDOUT_FILENO,"\n",1);
            }
            else{
                lseek(miner_sursa, v[i], SEEK_SET);
                copiat = read(miner_sursa, buf, v[i+1] - v[i]);
                copiat = write(STDOUT_FILENO,buf,copiat);
            }
        }


     
        close(miner_sursa);
        return 0;
    }

