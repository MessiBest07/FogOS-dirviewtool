#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAX 512

#define KB 1024
#define MB 1048576

void print_size(int size) {
  if (size < KB) {
    printf("%d B\n", size);
  } else if (size < MB) {
    printf("%d KB\n", size / KB);
  } else {
    printf("%d MB\n", size / MB);
  }
}


void du(const char *path, int whole_directory, int sort_files) {
    int fd;
    struct stat st;
    struct dirent de;
    int total_size = 0;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "du: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "du: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_DIR) {
        char buf[MAX], *p;
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("du: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR) {
                du(buf, whole_directory, sort_files); 
            }
            total_size += st.size;
        }
        if (whole_directory) {
            printf("%s total ", path);
            print_size(total_size);
        }
    } else {

        printf("%s ", path);
        print_size(st.size);
    }
    close(fd);
}



int main(int argc, char *argv[]) {
    int whole_directory = 0;  
    int sort_files = 0;      

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            whole_directory = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            sort_files = 1;
        } else {
            du(argv[i], whole_directory, sort_files);
        }
    }
    exit(0);
}


