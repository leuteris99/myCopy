#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define O_BINARY  0


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wnon-literal-null-conversion"

int myCpy(int dest_file, char *data, int wrote);

size_t getFileSize(const char *filename);

int main(int argc, char **argv) {
    int buf_size = 1024;
    char *src_file = argv[1];
    char *dest_file = argv[2];
    char *c = calloc(buf_size, 8);

    int fd = open(src_file, O_RDONLY | O_BINARY);
    int ff = open(dest_file, O_WRONLY | O_CREAT | O_BINARY);
    printf("%d", ff);


    int sm = access(dest_file, W_OK);
    if (sm < 0) {
        chmod(dest_file, W_OK);
    }
    sm = access(dest_file, W_OK);
    printf("sm: %d", sm);


    if (fd < 0) {
        printf("Something went wrong while reading the file: %s", src_file);
        return -1;
    }
    if (ff < 0) {
        printf("Something went wrong while reading the file: %s %d", dest_file, ff);
        return -1;
    }


    size_t file_size = getFileSize(src_file);
    printf("%zu", file_size);

    int wrote = 0;
    double rep = (file_size) / (double) (buf_size);

    for (int i = 0; i < rep; ++i) {
        if (lseek(fd, wrote, SEEK_SET) < 0) {
            printf("error: lseek");
        }
        int rc = read(fd, c, buf_size);
        if (rc < 0) {
            printf("error: reading data");
        }
        //printf("%s", c);
        myCpy(ff, c, wrote);
        wrote += buf_size;
    }
    lseek(ff, 0L, SEEK_END);
    int wc = write(ff, "\0", 1);
    if (wc < 0) {
        printf("error: writing data");
    }
    if (close(fd) < 0) {
        printf("error: closing src file");
    }
    if (close(ff) < 0) {
        printf("error: closing dest file");
    }
    //printf("\n\n");
    return 0;
}

int myCpy(int dest_file, char *data, int wrote) {
    lseek(dest_file, (long)wrote, SEEK_SET);
    if (write(dest_file, data, strlen(data)) < 0) {
        printf("error: write data");
    }
}

size_t getFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

#pragma clang diagnostic pop