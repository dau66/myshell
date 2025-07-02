#include <shell.h>

Command* create_command_node(void) {
    Command* new_cmd = (Command*)malloc(sizeof(Command));
    if (new_cmd == NULL) {
        perror("Failed to allocate memory for Command node");
        return NULL;
    }
    // 全てのポインタをNULLに初期化
    new_cmd->argv = NULL;
    new_cmd->redirect_in = NULL;
    new_cmd->redirect_out = NULL;
    new_cmd->append_mode = T_WORD; // デフォルト値（リダイレクトなしを示す）
    new_cmd->heredoc_delimiter = NULL;
    new_cmd->next = NULL;
    return new_cmd;
}