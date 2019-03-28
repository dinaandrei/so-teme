#include <stdio.h>
#include <string.h>
#include <fcntl.h>


int main(){

    int fd = open("a.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    write(fd, "ala", 3);
    write(fd, " p", 2);

    close(fd);
    return 0;
}