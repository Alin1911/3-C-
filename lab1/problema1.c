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
        int copiat;
        char buf[1024];
     
        miner_sursa = open("sursa.txt", O_RDONLY);
     
        if (miner_sursa < 0)
            fatal("Nu pot deschide un fisier");
     
        lseek(miner_sursa, 0, SEEK_SET);
     
        while ((copiat = read(miner_sursa, buf, sizeof(buf)))) {
            if (copiat < 0)
                fatal("Eroare la citire");
            copiat = write(STDOUT_FILENO, buf, copiat);
            if (copiat < 0)
                fatal("Eroare la scriere");
        }
     
        close(miner_sursa);
        return 0;
    }

