#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "so_stdio.h"

#define B_SIZE 4096

mode_t get_file_opening_flags(const char *mode);

mode_t get_file_opening_flags(const char *mode)
{

    if (strncmp(mode, "r", 1) == 0)
        return O_RDONLY;

    if (strncmp(mode, "r+", 2) == 0)
        return O_RDWR;

    if (strncmp(mode, "w", 1) == 0)
        return O_WRONLY | O_CREAT | O_TRUNC;

    if (strncmp(mode, "w+", 2) == 0)
        return O_RDWR | O_CREAT | O_TRUNC;

    if (strncmp(mode, "a", 1) == 0)
        return O_APPEND | O_CREAT;

    if (strncmp(mode, "a+", 2) == 0)
        return O_APPEND | O_RDONLY | O_CREAT;

    return 0x6969;
}

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    mode_t i = get_file_opening_flags(mode);
    if(i == 0x6969)
        return NULL;
    int fd = open(pathname, i, 0644);
    if (fd < 1)
        return NULL;

    SO_FILE *s_file = malloc(sizeof(SO_FILE));
    s_file->fd = fd;
    s_file->buffer_read = malloc(sizeof(char) * B_SIZE);
    s_file->buffer_write = malloc(sizeof(char) * B_SIZE);
    s_file->read_pos = 0;
    s_file->write_pos = 0;
    s_file->buff_read_len = 0;
    s_file->buff_write_len = 0;
    s_file->is_at_end_read = 0;

    return s_file;
}

int so_fileno(SO_FILE *stream){
    if(stream == NULL)
        return SO_EOF;
    
    return stream->fd;
}

int so_fclose(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        return SO_EOF;
    }

    if(stream->write_pos > 0 && so_fflush(stream) < 0 ){
        return SO_EOF;
    }

    int status = close(stream->fd);
    free(stream->buffer_read);
    free(stream->buffer_write);
    free(stream);

    if (status == 0)
        return status;

    return SO_EOF;
}

int so_fgetc(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
        return SO_EOF;

    if(stream->is_at_end_read == 1 && stream->buffer_read[stream->read_pos] == '\0')
        return SO_EOF;

    if(stream->read_pos == stream->buff_read_len){
        int status = read(stream->fd, stream->buffer_read, B_SIZE);
        if(status < 0)
            return SO_EOF;

        stream->buff_read_len = status;
        stream->read_pos = 0;

        if(status == 0){
            stream->buff_read_len = 4096;
            stream->is_at_end_read = 1;
        }
    }

    return (int)stream->buffer_read[stream->read_pos++];
}

int write_free_buffer(SO_FILE * stream){
    int status = write(stream->fd, (void*) stream->buffer_write, stream->buff_write_len);
    if(status < 0) 
        return SO_EOF;

    strcpy(stream->buffer_write, "");
    stream->buff_write_len = 0;
    stream->write_pos = 0;

    return status;
}

int so_fflush(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
        return SO_EOF;

    int status = write_free_buffer(stream);
    return status;
    
}

int so_fputc(int c, SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
        return SO_EOF;

    if(stream->write_pos == B_SIZE){
        int status = write_free_buffer(stream);
        if(status < 0) 
            return SO_EOF;
    }
    stream->buffer_write[stream->write_pos++] = c;
    stream->buff_write_len ++;
    
    return c;

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
    void *buf = malloc(size * nmemb);
    int c, i;
    int initial_pos = stream->read_pos;
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
