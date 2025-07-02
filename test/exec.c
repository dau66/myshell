#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("execvpを呼び出す前です。\n");

    char *args[] = {"ls", "-l", NULL}; // 引数配列を作成

    // PATH環境変数を使って "ls" を検索し、args配列の引数で実行
    execvp("ls", args);

    // execvpが成功した場合、ここには到達しない
    perror("execvpの呼び出しに失敗しました");
    return 1;
}