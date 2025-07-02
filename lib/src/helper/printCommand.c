#include <shell.h>

// Command構造体の内容を表示するヘルパー関数 (デバッグ用)
void print_command_list(Command* head) {
    int cmd_index = 0;
    Command* current_cmd = head;
    while (current_cmd != NULL) {
        printf("--- Command %d ---\n", cmd_index++);
        printf("  argv: { ");
        if (current_cmd->argv) {
            for (int i = 0; current_cmd->argv[i] != NULL; i++) {
                printf("\"%s\"%s", current_cmd->argv[i], current_cmd->argv[i+1] != NULL ? ", " : "");
            }
        }
        printf(" }\n");
        printf("  redirect_in: %s\n", current_cmd->redirect_in ? current_cmd->redirect_in : "(null)");
        printf("  redirect_out: %s (Mode: %s)\n", 
               current_cmd->redirect_out ? current_cmd->redirect_out : "(null)",
               current_cmd->redirect_out ? token_type_to_string(current_cmd->append_mode) : "(N/A)");
        printf("  heredoc_delimiter: %s\n", current_cmd->heredoc_delimiter ? current_cmd->heredoc_delimiter : "(null)");
        
        current_cmd = current_cmd->next;
        if (current_cmd != NULL) {
            printf("  -> Piped to next command\n");
        }
    }
} 