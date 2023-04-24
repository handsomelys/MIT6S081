#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

char std_in[512];
// pay attention!!!!
// in "xargs", the parameters is only the part of xargs ...
// e.g.
// in command: find . b | xargs grep hello , the argv just "xargs grep hello"
int main(int argc, char* argv[]) {

    int std_in_len = read(0, std_in, sizeof(std_in));
    int line = 0;
    // get the argument by lines
    // actually, the std_in stored the left end of the command.
    // e.g. "find . b" 's result
    for (int i = 0; i < std_in_len; i++) {
        if (std_in[i] == '\n') {
            line++;
        }
    }


// store the output of the left end by lines
/*
./a/b
./c/b
./b
*/
char exec_argv[line][MAXARG];
int cur_row = 0, cur_char_idx = 0;

    for (int i = 0; i < std_in_len; i++) {
        if (std_in[i] == '\n') {
            exec_argv[cur_row][cur_char_idx] = 0;    // if current char is \n, replace it with 0, because in C, '0' means the string's end.
            cur_row++;
            cur_char_idx = 0;
        }
        else {
            exec_argv[cur_row][cur_char_idx] = std_in[i];
            cur_char_idx++;
        }
    }
    

    // a array of strings. a string means a command that concat after xargs.
    char* x_args[MAXARG];

    // argv[0] means the command, so just store the 1 ~ argc - 1 of the argv
    for (cur_char_idx = 0; cur_char_idx < argc - 1; cur_char_idx++) {
        x_args[cur_char_idx] = argv[cur_char_idx + 1];
    }

    int concat_idx = cur_char_idx;
    cur_row = 0;
    while (cur_row < line) {
        // concat the augment to the command's end
        x_args[concat_idx] = exec_argv[cur_row++];
        int pid = fork();
        if (pid < 0) {
            printf("fork error!\n");
            exit(1);
        } else if (pid == 0) {
            exec(argv[1], x_args);
        } else {
            wait((int*)0);
        }
    }
    exit(0);
}