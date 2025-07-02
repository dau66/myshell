#include <unistd.h> // fork(), execvp(), wait()
#include <stdio.h>  // printf, perror
#include <stdlib.h> // exit

int main() {
    pid_t pid;
    char *argv[] = {"ls", "-l", NULL}; // 実行したいコマンドと引数

    printf("親プロセス開始 (PID: %d)\n", getpid());

    pid = fork(); // 子プロセスを作成

    if (pid == -1) {
        perror("fork 失敗");
        return 1;
    } else if (pid == 0) {
        // --- 子プロセス ---
        printf("子プロセス開始 (PID: %d)。コマンド '%s' を実行します。\n", getpid(), argv[0]);
        execvp(argv[0], argv); // コマンドを実行（成功すればここから下は実行されない）

        // execvp が失敗した場合のみここに到達
        perror("execvp 失敗");
        exit(1); // エラー終了
    } else {
        // --- 親プロセス ---
        int status;
        printf("親プロセスは子プロセス (PID: %d) の終了を待っています。\n", pid);
        wait(&status); // 子プロセスの終了を待つ

        printf("子プロセス (PID: %d) が終了しました。ステータス: %d\n", pid, WEXITSTATUS(status));
        printf("親プロセス終了 (PID: %d)\n", getpid());
    }

    return 0;
}