#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "so_stdio.h"

mode_t get_file_opening_flags(const char *mode);

mode_t get_file_opening_flags(const char *mode)
{

    if (strcmp(mode, "r"))
        return O_RDONLY;

    if (strcmp(mode, "r+"))
        return O_RDWR;

    if (strcmp(mode, "w"))
        return O_WRONLY | O_CREAT | O_TRUNC;

    if (strcmp(mode, "w+"))
        return O_RDWR | O_CREAT | O_TRUNC;

    if (strcmp(mode, "a"))
        return O_APPEND | O_CREAT;

    if (strcmp(mode, "a+"))
        return O_APPEND | O_RDONLY | O_CREAT;

    return (mode_t)-1;
}

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    printf("asgdsgsdgsdrgsdrg");
    mode_t i = get_file_opening_flags(mode);
    if(i == -1)
        return NULL;
    int fd = open(pathname, i, 0644);
    if (fd < 1)
        return NULL;

    SO_FILE *s_file = malloc(sizeof(SO_FILE));
    s_file->fd = fd;
}

int so_fclose(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        return SO_EOF;
    }

    int status = close(stream->fd);
    free(stream);
    if (status == 0)
        return status;

    return SO_EOF;
}

int so_fgetc(SO_FILE *stream)
{

    if (stream == NULL || stream->fd < 0)
        return -1;

    char c;
    int status = read(stream->fd, &c, 1);
    if(status < 0)
        return -1;

    if(status == 0) 
        return SO_EOF;

    return (int)c;

}

int so_fputc(int c, SO_FILE *stream)
{

}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
}

long so_ftell(SO_FILE *stream)
{
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

    int num = 0;
    void *buf = malloc(sizeof(void) * size * nmemb);
    int c, i;

    do
    {

        for (i = 0; i < size; i++)
        {
            c = so_fgetc(stream);
            if (c == SO_EOF)
            {
                return 0;
            }
            strcat(buf, (char)c);
        }
        num++;
    } while (num < nmemb);

    strcpy(ptr, buf);
    return (size_t)num;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

    int num = 0;
    void *buf = malloc(sizeof(void) * size * nmemb);
    int c, i;

    do
    {

        for (i = 0; i < size; i++)
        {
            c = so_fputc(stream);
            if (c == SO_EOF)
            {
                return 0;
            }
            strcat(buf, (char)c);
        }
        num++;
    } while (num < nmemb);

    strcpy(ptr, buf);
    return (size_t)num;
}

int so_fflush(SO_FILE *stream)
{
}

int so_feof(SO_FILE *stream)
{
}

int so_ferror(SO_FILE *stream)
{
}

SO_FILE *so_popen(const char *command, const char *type)
{
}

int so_pclose(SO_FILE *stream)
{
}
