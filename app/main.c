#include <shell.h>

int main() {
    // --- 1. 初期化 ---
    // 設定ファイルの読み込みなど（高度な機能）
	shell_animation();
    // --- 2. メインループ ---
    while (1) {
    	char *line = readline("myshell> ");
        if (!line) {
            if (errno) {
                perror("readline");
                return EXIT_FAILURE;
            }
            printf("\nexit\n");
			exit(EXIT_SUCCESS);
            break; 
        }
		if(line[0] != '\0'){
			add_history(line);
		}
		Command* parsed = parser(line);
        print_command_list(parsed);
        free_command_list(parsed);
		free(line);
    }

    // --- 3. 終了処理 ---
    // 通常はループを抜けないが、必要ならここに記述

    return EXIT_SUCCESS;
}