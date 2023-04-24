#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 必须以这个顺序include，由于三个头文件有依赖关系

int main(int argc, char **argv) {
    if (argc < 2) {
        // 表示当前命令行参数只有一个 还需要sleep的时间
        printf("usage: sleep <ticks>\n");
    }
    // example:
    // sleep <ticks>
    // argv[1] means the <ticks>
    sleep(atoi(argv[1]));
    exit(0);
}