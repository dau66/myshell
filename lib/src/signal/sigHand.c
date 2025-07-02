#include <shell.h>

void signal_handler(int signum)
{
    /* シグナルをキャッチしたときに実行したい内容 */
    puts("SIGINT");
    exit(1);
}