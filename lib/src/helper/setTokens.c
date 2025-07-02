#include <shell.h>

void free_split_tokens(char** tokens, size_t num_tokens) { // num_tokens を size_t に変更
    if (tokens == NULL) {
        return;
    }
    for (size_t i = 0; i < num_tokens; i++) { // iをsize_t型に
        free(tokens[i]); 
    }
    free(tokens); 
}

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
    new_token->value = strdup(str); // 文字列を複製
    if (new_token->value == NULL) {
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
        free(current->value); // strdupで複製した文字列を解放
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