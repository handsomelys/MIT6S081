#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int p2c[2]; // 0 for read, 1 for write, the c2p is the same
    int c2p[2];

    // create two pipe for bidirectional communication
    pipe(p2c);  
    pipe(c2p);

    char buf_ping[4];
    char buf_pong[4];

    int pid = fork();
    if (pid > 0) {  //  parent process
        close(p2c[0]);  // close(release) the parent to child 'read'
        close(c2p[1]);  // close(release) the child to parent 'write'
        
        // the parent send "ping" to child with p2c[1] (means write the pipe)
        write(p2c[1], "ping", strlen("ping"));
        // wait until the child process returns
        wait((int*)0);
        // read from c2p[0], and compare the ret to the "pong", if true, printf.
        if (read(c2p[0], buf_pong, sizeof(buf_pong)) == 4 && strcmp(buf_pong, "pong") == 0) {
            printf("%d: received pong\n", getpid());
        }

        close(p2c[1]);  // close(release) the p2c write.
        close(c2p[0]);  // close(release) the c2p read.
        exit(0);    // means success
    } else if (pid == 0) {    // child process
        close(c2p[0]);
        close(p2c[1]);
        if (read(p2c[0], buf_ping, sizeof(buf_ping)) == 4 && strcmp(buf_ping, "ping") == 0) {
            printf("%d: received ping\n", getpid());
        }
        write(c2p[1], "pong", strlen("pong"));

        close(c2p[1]);  // close(release) the fd about child write to parent
        close(p2c[0]);  // close(release) the fd about parent read from child

        exit(0);    // means success
    } else {
        printf("fork error...\n");
        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        exit(1);
    }
}