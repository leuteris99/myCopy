#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define O_BINARY  0


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wnon-literal-null-conversion"

int myCpy(int dest_file, char *data, int wrote);

size_t getFileSize(const char *filename);

int charArToInt(char *array);

int main(int argc, char **argv) {
    char *src_file; // where to copy from
    char *dest_file; // where to copy to
    bool cbc = false; // custom buffer checker
    int buf_size = 4096; // buffer size: default to 4kB

    switch (argc) { // error check the given args
        case 0:
        case 1:
            perror("No arguments.\n./main <options> [src_file] [dest_file]\nOptions:\n-b <size> : sets a custom size buffer.");
            exit(-1);
            break;
        case 2:
            if (strcmp(argv[1], "-b") == 0) {
                perror("No arguments.\n./main <options> [src_file] [dest_file]\nOptions:\n-b <size> : sets a custom size buffer.");
                exit(-2);
            } else {
                src_file = argv[1];
                dest_file = "copiedFile";
            }
            break;
        case 3:
            if (strcmp(argv[1], "-b") == 0) {
                perror("Arguments error.\n./main <options> [src_file] [dest_file]\nOptions:\n-b <size> : sets a custom size buffer.");
                exit(-5);
            } else {
                src_file = argv[1];
                dest_file = argv[2];
            }
            break;
        case 4:
            if (strcmp(argv[1], "-b") == 0) {
                src_file = argv[3];
                dest_file = "copiedFile";
                cbc = true;
            } else {
                perror("Argument format Error:\n./main <options> [src_file] [dest_file]\nOptions:\n-b <size> : sets a custom size buffer.");
                exit(-6);
            }
            break;
        case 5:
            if (strcmp(argv[1], "-b") == 1) {
                perror("Options error:\n/main <options> [src_file] [dest_file]\nOptions:\n-b <size> : sets a custom size buffer.");
                exit(-3);
            } else {
                src_file = argv[3];
                dest_file = argv[4];
                cbc = true;
            }
            break;
        default:
            perror("error: ");
            exit(-4);
            break;
    }

    if (cbc) { // if there is a custom buffer size set the new buffer size
        buf_size = charArToInt(argv[2]);
    }
    char *c = malloc(buf_size); // allocate the memory to read to and write from

    int fd = open(src_file, O_RDONLY | O_BINARY); // open the source file
    int ff = open(dest_file, O_WRONLY | O_CREAT | O_BINARY); // open the dest file

    int sm = access(dest_file, W_OK); // check for access granted
    if (sm < 0) {
        chmod(dest_file, W_OK); // chmod if no access
    }
    sm = access(dest_file, W_OK);

    if (fd < 0) { // check for some more errors
        perror("Something went wrong while reading the src file.");
        exit(3);
    }
    if (ff < 0) {
        perror("Something went wrong while reading the dest file.");
        exit(4);
    }

    size_t file_size = getFileSize(src_file); // get them file sizes

    int wrote = 0; // pointer for the lseek
    double rep = (file_size) / (double) (buf_size); // how many times to repeat

	// starting the timer
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);
  
    for (int i = 0; i < rep; ++i) { // main copy loop
        if (lseek(fd, wrote, SEEK_SET) < 0) { //check for seek error
            perror("error: lseek");
            exit(5);
        }
		if (i + 1 > rep ) { // for the last loop pass get a new string
			char * cr = malloc((int)file_size - wrote);
			int rc = read(fd, cr, buf_size);
			printf("lastone");
			if (rc < 0) {
				perror("error: reading data");
				exit(6);
			}
			myCpy(ff, cr, wrote); 
			break;
		}
        int rc = read(fd, c, buf_size); // read from the file
        if (rc < 0) { // if error
            perror("error: reading data");
            exit(6);
        }
        myCpy(ff, c, wrote); // copy
        wrote += buf_size; // increase the value of seek pointer by buffer size
    }
    lseek(ff, 0L, SEEK_END);
    int wc = write(ff, "\0", 1);
    if (wc < 0) {
        perror("error: writing data");
        exit(7);
    }
    // stop the timer
    // speed of copy operation
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    //printf("Time elapsed: %ld.%06ld\n", (long int) tval_result.tv_sec, (long int) tval_result.tv_usec); //print the time of execution

    double speed = ((double) file_size / (double) (tval_result.tv_usec / 1000)) * 1000;
    printf("The speed of the buffer with size %d is %f B/s", buf_size, speed);

    if (close(fd) < 0) {// close the files , and always check for errors
        perror("error: closing src file");
        exit(8);
    }
    if (close(ff) < 0) {
        perror("error: closing dest file");
        exit(9);
    }

    printf("\n\n");
    return 0;
}

int myCpy(int dest_file, char *data, int wrote) { // where to write to , what and at which point in the file
    lseek(dest_file, (long) wrote, SEEK_SET);
    if (write(dest_file, data, strlen(data)) < 0) { // write and check for errors
        perror("error: write data");
        exit(10);
    }
}

size_t getFileSize(const char *filename) { // file name
    struct stat st; 
    if (stat(filename, &st) != 0) { // if empty terminate
        return 0;
    }
    return st.st_size; // get that size
}

int charArToInt(char *array){ // to_int function, for the terminal-passed buffer size argument
    return atoi(array);
}

#pragma clang diagnostic pop
