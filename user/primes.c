#include "kernel/types.h"

#include "user/user.h"

void prime(int* p);

int main(int argc, char* argv[]) {
    int p[2];
    pipe(p);

    // write the number to the left pipe
    // after this piece of shit code, the p[1] can be drop because there has no write operation later
    for (int number = 2; number <= 35; number++) {
        write(p[1], &number, sizeof(number));
    }
    close(p[1]);

    int pid = fork();
    if (pid < 0) {
        printf("fork error !\n");
        close(p[0]);
        exit(1);
    } else if (pid > 0) {   // parent process wait the result from children
        // close p[0]  cause it does not use
        close(p[0]);
        // wait's parameters why (int*)0?
        wait((int*)0);
        exit(0);
    } else {
        // according to the prime function, it does not use the write but read from the p.
        prime(p);
        close(p[0]);
        exit(0);
    }
}

void prime(int* p) {
    int prime_num;

    // during the recurrent, the int* p will be change, every p from the current process's parent process
    // each time, read a int type number from the pipe
    int len =  read(p[0], &prime_num, sizeof(prime_num));

    if (len == 0) {
        // if the len is 0, means the prime number is read done.
        close(p[0]);
        exit(0);
    } else {
        printf("prime %d\n", prime_num);
    }

    int num;
    int p_next[2];
    pipe(p_next);
    int pid = fork();

    if (pid == 0) { 
        // child process doesn't read from p but p_next
        close(p[0]);
        // child process doesn't write to p_next either
        close(p_next[1]);
        // child print the prime number till the pipe is empty
        prime(p_next);  
        // done the read task
        close(p_next[0]);
        exit(0);
    } else if (pid > 0) {
        // parent process read from p, judge the num is prime or not, and then write to p_next
        close(p_next[0]);
        // judge and write
        while (read(p[0], &num, sizeof(num))) {
            if (num % prime_num != 0) {
                write(p_next[1], &num, sizeof(num));
            }
        }
        close(p[0]);
        close(p_next[1]);
        wait((int*)0);  // the parent should wait, or it will meet some problems in concurrent
        exit(0);
    } else {
        printf("fork error !\n");
        close(p[0]);
        close(p_next[0]);
        close(p_next[1]);
        exit(1);
    }
}

/*

*/