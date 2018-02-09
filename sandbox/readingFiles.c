/*
* Linux Program to Read from a File using File Descriptors.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int fd,retval;
    char *buffer;

    if (argc < 2) {
        printf("Usage : %s pathname\n", argv[0]);
        exit(1);
    }

    /*opening the file in read-only mode*/
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("Problem in opening the file");
        exit(1);
    }

    /*reading bytes from the fd and writing it to the buffer*/
    // while ((retval = read(fd, buffer, 1)) > 0)
    //     printf("%c", *buffer);
    if (retval < 0) {
        perror("\nRead failure");
        exit(1);
    }

    char c;
    while()
    retval = read(fd,&c,1);
    printf("%c", *c);


    printf("Successfully read bytes from the file %s\n", argv[1]);

    close(fd);
}
