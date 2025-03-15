#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#endif

void PX_ProcessRunOnce(const char* name) 
{
#ifdef _WIN32
    char mutex_name[256];
    snprintf(mutex_name, sizeof(mutex_name), "Global\\%s", name); 
    HANDLE mutex = CreateMutex(NULL, TRUE, mutex_name);
    if (mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        exit(0);
    }
#else
    char lock_file[256];
    snprintf(lock_file, sizeof(lock_file), "/tmp/%s.lock", name);
    int fd = open(lock_file, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("open");
        exit(0);
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            exit(0);
        }
        perror("fcntl");
        close(fd);
        exit(0);
    }
#endif
    return;
}
