#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
copy match regular code from grep.c
*/
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}



void find(char* path, char* target);


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("useage: find <PATH> <FILENAME>\n");
        exit(1);
    }
    else {
        find(argv[1], argv[2]);
    }
    exit(0);
}

void find(char* path, char* target) {
    char buf[512]; 
    char* p;
    int fd, fd_current;
    struct dirent de;
    struct stat st, st_current;

    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return ;
    }

    switch(st.type) {
        //   if current file's type is file, error. cause the path should be dir.
        case T_FILE:
            {
                printf("%s is not a directory!\n", path);
                close(fd);
                break;
            }

        case T_DIR:
            {
                if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
                    printf("find: path too long\n");
                    break;
                }
                //  cpy the path to the buffer
                strcpy(buf, path);
                p = buf + strlen(buf);
                *p++ = '/'; // p points to the buffer's end
                while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                    //  a 'de' means a file in current file which the fd points to.
                    if (de.inum == 0) {
                        // de.inum == 0, means the file is not saved. it is invalid.
                        continue;
                    }

                    //  if the file name is "." or "..", continue
                    if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
                        continue;
                    }
                    // concate the de.name to the p's end
                    memmove(p, de.name, DIRSIZ);
                    if ((fd_current = open(buf, 0)) >= 0)
                    //  buf points to the current file in current path now.
                    {
                        if (fstat(fd_current, &st_current) >= 0) {
                            //  if valid
                            if (st_current.type == T_FILE) {
                                //  if the type of the file is T_FILE
                                // if (strcmp(de.name, target) == 0) {
                                //     printf("%s\n", buf);
                                // }

                                //  modify to match the regex expressions. 
                                if (match(de.name, target)) {
                                    printf("%s\n", buf);
                                }
                                close(fd_current);
                            } else if (st_current.type == T_DIR) {
                                //  if the type of the file is T_DIR, recur this function.
                                close(fd_current);
                                find(buf, target);
                            } else if (st_current.type == T_DEVICE) {
                                close(fd_current);
                            }
                        }
                    }
                    
                }
                break;
            }
    }
    close(fd);
}

