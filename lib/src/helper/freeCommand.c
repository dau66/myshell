#include <shell.h>
void free_command(Command* cmd) {
    if (cmd == NULL) {
        return;
    }
    // argv配列内の各文字列を解放
    if (cmd->argv) {
        for (int i = 0; cmd->argv[i] != NULL; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv); // argv配列自体を解放
    }
    free(cmd->redirect_in);
    free(cmd->redirect_out);
    free(cmd->heredoc_delimiter);
    free(cmd); // Command構造体自体を解放
}

 
void free_command_list(Command* head) {
    Command* current = head;
    while (current != NULL) {
        Command* next_cmd = current->next;
        free_command(current);
        current = next_cmd;
    }
}