#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path) //  format output the name of the file (without the parent dir)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de; // contains the files' inode number and file name.
  struct stat st; // contains the store files' base informations. eg. inode block number, file type, quote link number, file size...

// open returns the fd if success, else return -1
  if((fd = open(path, 0)) < 0){ //  open the file pointed by the fd, return the opened file's fd
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      // if path/filename/ length out of the buffer's size
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);  // the p point to the  path's tail of the buffer, and ready to add
    *p++ = '/'; //  add the / after the directory
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      //  read de once from directory till the read fail
      if(de.inum == 0)  // de.inum == 0, means the file didn't save. it is a invalid file
        continue;
      memmove(p, de.name, DIRSIZ);  //  write DIRSIZ content from de.name to p
      p[DIRSIZ] = 0;  //  write 0 in the end stands for the string ending
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
