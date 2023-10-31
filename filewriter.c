/*
   2023-10-31 Robert Martin-Legene

   Filewriter:
   Stdin is being appended to a file. When the output file disappears
   (or changes inode), the file descriptor will be closed and reopened
   (possibly created) and appending will happen to the new file.

   Filewriter works with any kind of data (treats input and output as
   binary octets) and does no buffering, because it uses read(2) and
   write(2).
*/

/* _LARGEFILE64_SOURCE is good on Linux, to allow opening very big files
   If this breaks your build process, then check the man page for open(2) */
#define _LARGEFILE64_SOURCE

#include <unistd.h>     /* fstat, stat, read, write, close */
#include <sys/types.h>  /* fstat, stat, open */
#include <sys/stat.h>   /* fstat, stat, open */
#include <fcntl.h>      /* open */
#include <stdlib.h>     /* exit */
#include <stdio.h>      /* perror, fprintf */

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv)
{
    char    buffer[8192];
    int     fd          =   -1;
    size_t  size        =   (size_t)sizeof(buffer);
    ssize_t bytesread   =   1;
    ssize_t writtenthistime;
    int     byteswritten;
    struct  stat    statbuf;
    ino_t   inode_openedfile;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        fprintf(stderr, "Writes everything from stdin to <filename> and "
            "reopens the output file if it disappears or changes inode\n");
        exit(1);
    }

    while (1)
    {
        bytesread = read(0, (void*)buffer, size);
        if (bytesread == 0) /* eof */
        {
            /* we're being polite, so we close properly */
            if (fd > -1)
                if (close(fd) == -1)
                    error(argv[0]);
            break;
        }
        if (bytesread == -1) /* error */
            error(argv[1]);
        /* if the filename has changed inode, close it and reopen */
        if (fd > -1)
        {
            switch (stat(argv[1], &statbuf))
            {
                case -1:
                    /* Maybe the file was moved or whatever. Close and make a new one */
                    if (close(fd) == -1)
                        error(argv[0]);
                    fd = -1;
                    break;
                case 0: /* stat succeeded */
                    /* If the file has changed inode, close it, so we open a new one */
                    if (statbuf.st_ino != inode_openedfile)
                    {
                        if (close(fd) == -1)
                            error(argv[0]);
                        fd = -1;
                    }
                    break;
            }
        }
        /* open the outfile, if we don't have an active one */
        if (fd == -1)
        {
            fd = open(argv[1], O_CREAT|O_WRONLY|O_APPEND|O_LARGEFILE|O_NOCTTY, 0666);
            if (fd == -1) /* error */
                error(argv[1]);
            if (fstat(fd, &statbuf) == -1)
                error(argv[1]);
            inode_openedfile = statbuf.st_ino;
        }
        /* write the buffer to the outfile */
        byteswritten          =   0;
        while (byteswritten < bytesread)
        {
            writtenthistime = write(fd, (void*)buffer+byteswritten, bytesread - byteswritten);
            if (writtenthistime == -1) /* error */
                error(argv[1]);
            byteswritten += writtenthistime;
        }
    }
    return 0;
}
