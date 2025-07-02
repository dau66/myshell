#include <stdio.h>
#include <string.h> // strlen, strcpy, strcmp, strdup
#include <stdlib.h> // malloc, free, realloc
#include <ctype.h>  // isspace

// --- TokenTypeの定義 ---
typedef enum {
    T_WORD,          // コマンド名、引数、ファイル名など
    T_PIPE,          // |
    T_REDIR_IN,      // <
    T_REDIR_OUT,     // >
    T_REDIR_APPEND,  // >>
    T_HEREDOC,       // <<
    T_EOF            // 入力の終わり
} TokenType;

// --- Token構造体とその関連関数の定義 ---
typedef struct Token {
    char *str;           // トークンの文字列
    TokenType type;      // トークンの種類
    struct Token *next;  // 次のトークンへのポインタ
} Token;

// TokenTypeを文字列に変換するヘルパー関数 (デバッグ用)
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case T_WORD: return "T_WORD";
        case T_PIPE: return "T_PIPE";
        case T_REDIR_IN: return "T_REDIR_IN";
        case T_REDIR_OUT: return "T_REDIR_OUT";
        case T_REDIR_APPEND: return "T_REDIR_APPEND";
        case T_HEREDOC: return "T_HEREDOC";
        case T_EOF: return "T_EOF";
        default: return "UNKNOWN";
    }
}

/**
 * @brief 新しいTokenノードを作成するヘルパー関数
 * @param str トークンに格納する文字列
 * @param type トークンの種類
 * @return 新しく割り当てられたTokenポインタ。メモリ割り当て失敗時はNULL。
 */
Token* create_token_node(char* str, TokenType type) {
    Token* new_token = (Token*)malloc(sizeof(Token));
    if (new_token == NULL) {
        perror("Failed to allocate memory for Token node");
        return NULL;
    }
    new_token->str = strdup(str); // 文字列を複製
    if (new_token->str == NULL) {
        perror("Failed to duplicate token string");
        free(new_token);
        return NULL;
    }
    new_token->type = type;
    new_token->next = NULL;
    return new_token;
}

/**
 * @brief Token連結リストを解放するヘルパー関数
 * @param head 解放するリストの先頭ポインタ
 */
void free_token_list(Token* head) {
    Token* current = head;
    while (current != NULL) {
        Token* next_node = current->next;
        free(current->str); // strdupで複製した文字列を解放
        free(current);      // Tokenノード自体を解放
        current = next_node;
    }
}

/**
 * @brief 文字列配列をToken連結リストに変換し、TokenTypeを割り当てる
 *
 * @param str_array split_by_whitespaceから返された文字列の配列
 * @param count str_array内の要素数
 * @return 構築されたToken連結リストの先頭へのポインタ。失敗時はNULL。
 * 呼び出し側は、リストの解放責任を負う必要があります (free_token_list)。
 */
Token* tokenize_strings(char** str_array, size_t count) {
    if (str_array == NULL || count == 0) { // count <= 0 から count == 0 に修正
        return NULL;
    }

    Token* head = NULL;
    Token* current = NULL;

    for (size_t i = 0; i < count; i++) { // iをsize_t型に
        TokenType type;
        // 文字列の内容に基づいてTokenTypeを判定
        if (strcmp(str_array[i], "|") == 0) {
            type = T_PIPE;
        } else if (strcmp(str_array[i], "<") == 0) {
            type = T_REDIR_IN;
        } else if (strcmp(str_array[i], ">") == 0) {
            type = T_REDIR_OUT;
        } else if (strcmp(str_array[i], ">>") == 0) {
            type = T_REDIR_APPEND;
        } else if (strcmp(str_array[i], "<<") == 0) {
            type = T_HEREDOC;
        } else {
            type = T_WORD; // それ以外は通常の単語
        }

        Token* new_node = create_token_node(str_array[i], type);
        if (new_node == NULL) {
            // エラー発生時は、それまでに作成したリストを解放
            free_token_list(head); // 修正: headを使ってリスト全体を解放
            return NULL;
        }

        if (head == NULL) {
            head = new_node;
            current = new_node;
        } else {
            current->next = new_node;
            current = new_node;
        }
    }
    return head;
}


// --- split_by_whitespace と free_split_tokens のコード ---
char** split_by_whitespace(const char* str, size_t* num_tokens) { // num_tokens を size_t* に変更
    if (str == NULL || num_tokens == NULL) {
        return NULL;
    }

    char* str_copy = strdup(str); 
    if (str_copy == NULL) {
        return NULL; // メモリ割り当て失敗
    }

    char** tokens = NULL; 
    size_t count = 0;        // int から size_t に変更
    char* rest = str_copy; 
    char* token;

    token = strtok_r(rest, " \t\n\r\f\v", &rest); 
    while (token != NULL) {
        char** temp_tokens = (char**)realloc(tokens, (count + 1) * sizeof(char*));
        if (temp_tokens == NULL) {
            for (size_t i = 0; i < count; i++) { // iをsize_t型に
                free(tokens[i]); 
            }
            free(tokens);
            free(str_copy); 
            *num_tokens = 0;
            return NULL;
        }
        tokens = temp_tokens;

        tokens[count] = strdup(token);
        if (tokens[count] == NULL) {
            for (size_t i = 0; i < count; i++) { // iをsize_t型に
                free(tokens[i]); 
            }
            free(tokens);
            free(str_copy); 
            *num_tokens = 0;
            return NULL;
        }
        
        count++;
        token = strtok_r(rest, " \t\n\r\f\v", &rest);
    }

    free(str_copy);

    if (count == 0) {
        *num_tokens = 0;
        return NULL; 
    }

    *num_tokens = count;
    return tokens;
}

void free_split_tokens(char** tokens, size_t num_tokens) { // num_tokens を size_t に変更
    if (tokens == NULL) {
        return;
    }
    for (size_t i = 0; i < num_tokens; i++) { // iをsize_t型に
        free(tokens[i]); 
    }
    free(tokens); 
}


// --- Command構造体とその関連関数の定義 ---
typedef struct Command {
    char **argv;              // コマンドと引数の配列 (例: {"ls", "-l", NULL})
    char *redirect_in;        // 入力リダイレクトのファイル名
    char *redirect_out;       // 出力リダイレクトのファイル名
    TokenType append_mode;    // T_REDIR_OUT か T_REDIR_APPEND かを判別
    char *heredoc_delimiter;  // ヒアドキュメントの区切り文字
    struct Command *next;     // パイプで繋がる次のコマンド
} Command;

/**
 * @brief 新しいCommandノードを作成するヘルパー関数
 * @return 新しく割り当てられたCommandポインタ。メモリ割り当て失敗時はNULL。
 */
Command* create_command_node() {
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

/**
 * @brief Command構造体のメモリを解放するヘルパー関数
 * @param cmd 解放するCommand構造体へのポインタ
 */
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

/**
 * @brief Command連結リスト全体を解放するヘルパー関数
 * @param head 解放するリストの先頭ポインタ
 */
void free_command_list(Command* head) {
    Command* current = head;
    while (current != NULL) {
        Command* next_cmd = current->next;
        free_command(current);
        current = next_cmd;
    }
}

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
                temp_argv[argv_count] = strdup(current_token->str); // 文字列を複製
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
                    current_cmd->redirect_in = strdup(current_token->str);
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
                    current_cmd->redirect_out = strdup(current_token->str);
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
                    current_cmd->heredoc_delimiter = strdup(current_token->str);
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


// メイン関数（テスト用）
int main() {
    const char* test_commands[] = {
        "ls -l | grep .c > output.txt",
        "cat < input.txt >> log.txt",
        "echo hello << END_DELIM",
		"cat << itintin",
        "command_only",
        "  | invalid_pipe", // エラーケース
        "cmd arg <", // エラーケース
        "cmd arg > file > another", // エラーケース
        "cmd arg |", // エラーケース
        " | ", // エラーケース
        "", // 空文字列
        "   \t\n   " // 全て空白文字
    };
    int num_test_commands = sizeof(test_commands) / sizeof(test_commands[0]);

    for (int i = 0; i < num_test_commands; i++) {
        printf("========== Parsing Command: \"%s\" ==========\n", test_commands[i]);

        size_t num_split_tokens; // int から size_t に変更
        char** split_strings = split_by_whitespace(test_commands[i], &num_split_tokens);

        if (split_strings == NULL) { // num_split_tokens == 0 は split_strings == NULL のときに満たされるので不要
            printf("  -> No valid tokens found (empty or all whitespace input).\n");
            printf("===================================================\n\n");
            continue;
        }

        Token* token_list_head = tokenize_strings(split_strings, num_split_tokens);
        free_split_tokens(split_strings, num_split_tokens); // split_stringsはもう不要なので解放

        if (token_list_head == NULL) {
            printf("  -> Failed to create token list (empty or all whitespace).\n"); // メッセージを修正
            printf("===================================================\n\n");
            continue;
        }

        Command* command_list_head = parse_tokens_to_commands(token_list_head);
        
        if (command_list_head) {
            print_command_list(command_list_head);
            free_command_list(command_list_head);
        } else {
            printf("  -> Failed to parse commands (syntax error or empty command after parsing).\n"); // メッセージを修正
        }
        
        free_token_list(token_list_head); // Tokenリストも解放
        printf("===================================================\n\n");
    }

    return 0;
}