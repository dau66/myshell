#include <stdio.h>
#include <stdlib.h> // getenvのため (今回の主題とは直接関係ないが、環境変数関連でよく使う)

// environ グローバル変数を宣言
extern char **environ;

int main() {
    int i = 0;

    printf("現在の環境変数:\n");

    // environ 配列をNULLポインタが見つかるまでループ
    while (environ[i] != NULL) {
        printf("%s\n", environ[i]);
        i++;
    }

    return 0;
}