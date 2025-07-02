#include <shell.h>
/* "ls -l > out.txt | grep .c"
↓
["ls", "-l", ">", "out.txt", "|", "grep", ".c"] (トークンのリスト)

/**
 * @brief Token連結リストを解析し、Command構造体の連結リストを作成する
 *
 * @param tokens_head 解析するToken連結リストの先頭ポインタ
 * @return 構築されたCommand連結リストの先頭へのポインタ。失敗時はNULL。
 * 呼び出し側は、返されたリストの解放責任を負う必要があります (free_command_list)。
 */
Command* parse_tokens_to_commands(Token* tokens_head) {
    if (tokens_head == NULL) {
        return NULL; // 入力トークンがない場合はNULLを返す
    }

    Command* cmd_head = NULL;
    Command* current_cmd = NULL;
    Token* current_token = tokens_head;

    size_t argv_count = 0; // 現在のコマンドのargvの要素数 (intからsize_tに変更)
    char** temp_argv = NULL; // argvを動的に構築するための一時ポインタ

    // 最初のコマンドノードを作成
    cmd_head = create_command_node();
    if (cmd_head == NULL) return NULL;
    current_cmd = cmd_head;

    while (current_token != NULL) {
        switch (current_token->type) {
            case T_WORD:
                // T_WORDはコマンド名または引数
                temp_argv = (char**)realloc(temp_argv, (argv_count + 2) * sizeof(char*));
                if (temp_argv == NULL) {
                    perror("Failed to realloc argv");
                    free_command_list(cmd_head); // エラー時は全て解放
                    return NULL;
                }
                temp_argv[argv_count] = strdup(current_token->value); // 文字列を複製
                if (temp_argv[argv_count] == NULL) {
                    perror("Failed to strdup argv string");
                    // 既に割り当てたtemp_argv内の文字列を解放
                    for (size_t i = 0; i < argv_count; i++) free(temp_argv[i]); // iをsize_t型に
                    free(temp_argv);
                    free_command_list(cmd_head);
                    return NULL;
                }
                temp_argv[argv_count + 1] = NULL; // ヌル終端
                argv_count++;
                break;

            case T_REDIR_IN:
            case T_REDIR_OUT:
            case T_REDIR_APPEND:
            case T_HEREDOC: { // ブロック内で変数を宣言するため
                TokenType redirect_type = current_token->type; // リダイレクト記号のタイプを保存
                current_token = current_token->next; // 次のトークンがファイル名/区切り文字

                if (current_token == NULL || current_token->type != T_WORD) {
                    fprintf(stderr, "Syntax error: Expected filename or delimiter after redirection operator\n");
                    free_command_list(cmd_head);
                    return NULL;
                }

                if (redirect_type == T_REDIR_IN) {
                    if (current_cmd->redirect_in != NULL) { 
                        fprintf(stderr, "Syntax error: Duplicate input redirection\n");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                    current_cmd->redirect_in = strdup(current_token->value);
                    if (current_cmd->redirect_in == NULL) {
                        perror("Failed to strdup redirect_in string");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                } else if (redirect_type == T_REDIR_OUT || redirect_type == T_REDIR_APPEND) {
                    if (current_cmd->redirect_out != NULL) { 
                        fprintf(stderr, "Syntax error: Duplicate output redirection\n");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                    current_cmd->redirect_out = strdup(current_token->value);
                    if (current_cmd->redirect_out == NULL) {
                        perror("Failed to strdup redirect_out string");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                    // ここでリダイレクト記号自体のタイプを保存
                    current_cmd->append_mode = redirect_type; 
                } else if (redirect_type == T_HEREDOC) {
                    if (current_cmd->heredoc_delimiter != NULL) { 
                        fprintf(stderr, "Syntax error: Duplicate heredoc redirection\n");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                    current_cmd->heredoc_delimiter = strdup(current_token->value);
                    if (current_cmd->heredoc_delimiter == NULL) {
                        perror("Failed to strdup heredoc_delimiter string");
                        free_command_list(cmd_head);
                        return NULL;
                    }
                }
                break; // case T_REDIR_IN, T_REDIR_OUT, T_REDIR_APPEND, T_HEREDOC の共通処理の終わり
            }

            case T_PIPE:
                // パイプが見つかったら、現在のコマンドを終了し、次のコマンドを開始
                if (argv_count == 0) { // コマンドがないのにパイプがある場合
                    fprintf(stderr, "Syntax error: No command before '|'\n");
                    free_command_list(cmd_head);
                    return NULL;
                }
                current_cmd->argv = temp_argv; // 現在のコマンドのargvを確定
                
                Command* next_cmd_node = create_command_node();
                if (next_cmd_node == NULL) {
                    free_command_list(cmd_head);
                    return NULL;
                }
                current_cmd->next = next_cmd_node;
                current_cmd = next_cmd_node;
                
                // 次のコマンドのためにargv関連をリセット
                temp_argv = NULL;
                argv_count = 0;
                break;
            
            case T_EOF:
                // T_EOFはトークンリストの終わりを示すが、実際にはループ条件で処理されるため
                // ここに到達することは稀。到達しても何もしない。
                break;
        }
        current_token = current_token->next;
    }

    // ループ終了後、最後のコマンドのargvを確定
    // コマンドが一つもパースされなかった場合 (例: "|", " ", "") または最後のパイプの後ろにコマンドがない場合
    if (argv_count == 0 && current_cmd->argv == NULL) { // current_cmd->argv == NULLを追加
         fprintf(stderr, "Syntax error: No command found or invalid pipe placement\n");
         free_command_list(cmd_head);
         return NULL;
    }
    // 既にargvがセットされている場合（例: パイプで確定済み）はスキップ
    if (current_cmd->argv == NULL) {
        current_cmd->argv = temp_argv;
    }


    return cmd_head;
}

Command* parser(char* line){
	size_t num_split_tokens;
	char** split_strings = split_by_whitespace(line, &num_split_tokens);
	if (split_strings == NULL){
        printf("  -> No valid tokens found (empty or all whitespace input).\n");
        printf("===================================================\n\n");
		return NULL; //must modify
	}
    Token* token_list_head = tokenize_strings(split_strings, num_split_tokens);
    free_split_tokens(split_strings, num_split_tokens); // split_stringsはもう不
	if(!token_list_head){
		return NULL; //must modify
	}
	Command* command_list_head = parse_tokens_to_commands(token_list_head);
	if(!command_list_head){
		exit(EXIT_FAILURE); //must modify
	}
    return command_list_head;
}