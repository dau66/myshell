#include <shell.h>

void shell_animation(void){
    printf("シェルを起動しています");
    fflush(stdout); // 出力を即座に表示

    usleep(300000); // 0.3秒待機
    printf(".");
    fflush(stdout);

    usleep(300000); // 0.3秒待機
    printf(".");
    fflush(stdout);

    usleep(300000); // 0.3秒待機
    printf(".");
    fflush(stdout);

    usleep(1000000); // 1秒待機
    printf("\n"); // 改行
}