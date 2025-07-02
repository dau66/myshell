#include <shell.h>

void appendCommand(struct Command** head, char **argv, char *redirect_in, char *redirect_out) {
    struct Command* newCommand = create_command_node();
    if (newCommand == NULL) {
        return;
    }
    
    newCommand->argv = argv;
    newCommand->redirect_in = redirect_in;
    newCommand->redirect_out = redirect_out;
    
    if (*head == NULL) {
        *head = newCommand;
        return;
    }

    struct Command* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newCommand;
}