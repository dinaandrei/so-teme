#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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
    if (i == 0x6969)
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
    s_file->num_of_reads = 0;
    s_file->error = 0;
    s_file->eof_pos = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return s_file;
}

int so_fileno(SO_FILE *stream)
{
    if (stream == NULL)
    {
        stream->error = 1;
        return SO_EOF;
    }

    return stream->fd;
}

int so_fclose(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }

    if (stream->write_pos > 0 && so_fflush(stream) < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }

    int status = close(stream->fd);
    free(stream->buffer_read);
    free(stream->buffer_write);
    free(stream);

    if (status == 0)
        return status;

    stream->error = 1;
    return SO_EOF;
}

int so_fgetc(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }
    if (stream->is_at_end_read == 1 && stream->buffer_read[stream->read_pos] == '\0')
    {
        stream->error = 1;
        return SO_EOF;
    }

    if (stream->read_pos == stream->buff_read_len)
    {
        int status = read(stream->fd, stream->buffer_read, B_SIZE);
        if (status < 0)
        {
            stream->error = 1;
            return SO_EOF;
        }

        stream->num_of_reads++;
        stream->buff_read_len = status;
        stream->read_pos = 0;

        if (status == 0)
        {
            stream->buff_read_len = 4096;
            stream->is_at_end_read = 1;
        }
    }

    int result = (int)stream->buffer_read[stream->read_pos++];
    return result;
}

int write_free_buffer(SO_FILE *stream, void *buff, int len)
{
    int status = write(stream->fd, buff, len);
    if (status < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }

    strcpy(stream->buffer_write, "");
    stream->buff_write_len = 0;
    stream->write_pos = 0;
    return status;
}

int so_fflush(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }

    int status = write_free_buffer(stream, stream->buffer_write, stream->buff_write_len);
    int result = status < 0 ? SO_EOF : 0;
    return result;
}

int so_fputc(int c, SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
    {
        stream->error = 1;
        return SO_EOF;
    }

    if (stream->write_pos == B_SIZE)
    {
        int status = write_free_buffer(stream, stream->buffer_write, stream->buff_write_len);
        if (status < 0)
        {
            stream->error = 1;
            return SO_EOF;
        }
    }
    stream->buffer_write[stream->write_pos++] = (char)c;
    stream->buff_write_len++;

    return c;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
}

long so_ftell(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0){
        stream->error = 1;
        return SO_EOF;
    }

    return stream->read_pos + (stream->num_of_reads - 1) * B_SIZE;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    int total = size * nmemb;
    size_t num = 0;
    int pos = 0;

    while (total > 0)
    {
        printf("pos: %d\n", stream->read_pos);
        if (so_fgetc(stream) == SO_EOF && stream->read_pos == stream->eof_pos){
             stream->error = 1;
            return 0;
        }

        stream->read_pos--;
        int num_elems = stream->buff_read_len - stream->read_pos < total ? stream->buff_read_len - stream->read_pos : total;

        memcpy(ptr + pos, stream->buffer_read + stream->read_pos, num_elems);
        stream->read_pos += num_elems;
        total -= num_elems;
        num += num_elems / size;
        pos += num_elems;
    }
    return num;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    size_t total = size * nmemb;
    size_t num = 0;
    int i, pos = 0;
    char *buf = (char *)ptr;

    while (total > 0)
    {
        int c = so_fputc('!', stream);
        if (c == SO_EOF){
            stream->error = 1;
            return 0;
        }

        stream->buff_write_len--;
        stream->write_pos--;

        int num_elems = B_SIZE < total ? B_SIZE : total;
        for (i = 0; i < num_elems; i++)
        {
            so_fputc((int)buf[i + pos], stream);
        }

        num += num_elems / size;
        total -= num_elems;
        pos += num_elems;
    }

    return num;
}

int so_feof(SO_FILE *stream)
{
    if (stream == NULL || stream->fd < 0)
        return 0;

    int result = stream->eof_pos == ftell(stream) ? 1 : 0;
    return result;
}

int so_ferror(SO_FILE *stream)
{
    return stream->error;
}

SO_FILE *so_popen(const char *command, const char *type)
{
}

int so_pclose(SO_FILE *stream)
{
}
