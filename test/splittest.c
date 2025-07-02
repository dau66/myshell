#include <stdio.h>
#include <string.h> // for strtok_r, strlen, strcpy
#include <stdlib.h> // for malloc, free
#include <ctype.h>  // for isspace (間接的に必要)
// トークンを表現する構造体（連結リストで管理）
typedef struct Token {
    char *str;           // トークンの文字列
    TokenType type;      // トークンの種類
    struct Token *next;  // 次のトークンへのポインタ
} Token;

/**
 * @brief 文字列を空白区切りで分割し、分割されたトークンを動的配列で返す
 *
 * この関数は、入力文字列のコピーをとり、strtok_r を使用して空白でトークン化します。
 * 分割された各トークンは新しくメモリ割り当てされ、そのポインタが配列に格納されます。
 *
 * @param str 分割したい元の文字列。この文字列は変更されません。
 * @param num_tokens 分割されたトークンの数を格納するポインタ。
 * @return 分割されたトークン文字列のポインタ配列。
 * 文字列がNULL、num_tokensがNULL、メモリ割り当てに失敗した場合、
 * または文字列が空か全て空白文字(スペース、タブ、改行など)のみで構成されている場合は NULL。
 * 呼び出し側は、返された配列とその中の各文字列を free() する必要があります。
 */
char** split_by_whitespace(const char* str, int* num_tokens) {
    if (str == NULL || num_tokens == NULL) {
        return NULL;
    }

    // 元の文字列のコピーを作成 (strtok_rが元の文字列を変更するため)
    // strdupはmallocとstrcpyを組み合わせたもの。C99より後で標準化されたが、多くの環境で利用可能。
    char* str_copy = strdup(str); 
    if (str_copy == NULL) {
        return NULL; // メモリ割り当て失敗
    }

    char** tokens = NULL; // トークンを格納するポインタ配列
    int count = 0;        // トークン数
    char* rest = str_copy; // strtok_rの内部状態を管理するためのポインタ
    char* token;

    // トークンを取得
    // 区切り文字は一般的な空白文字: スペース, タブ, 改行, キャリッジリターン, フォームフィード, 垂直タブ
    token = strtok_r(rest, " \t\n\r\f\v", &rest); 
    while (token != NULL) {
        // reallocを使って配列のサイズを拡張
        char** temp_tokens = (char**)realloc(tokens, (count + 1) * sizeof(char*));
        if (temp_tokens == NULL) {
            // メモリ割り当て失敗時のクリーンアップ
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(str_copy); // 元の文字列のコピーも解放
            *num_tokens = 0;
            return NULL;
        }
        tokens = temp_tokens;

        // トークンを新しくメモリ割り当てしてコピー
        tokens[count] = strdup(token);
        if (tokens[count] == NULL) {
            // メモリ割り当て失敗時のクリーンアップ
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(str_copy); // 元の文字列のコピーも解放
            *num_tokens = 0;
            return NULL;
        }
        
        count++;
        token = strtok_r(rest, " \t\n\r\f\v", &rest);
    }

    // 最後に、元の文字列のコピーを解放
    free(str_copy);

    // トークンが一つも生成されなかった場合（空文字列、または全て空白文字の場合）はNULLを返す
    if (count == 0) {
        *num_tokens = 0;
        return NULL; 
    }

    *num_tokens = count;
    return tokens;
}

/**
 * @brief split_by_whitespace によって割り当てられたメモリを解放するヘルパー関数
 *
 * @param tokens split_by_whitespace から返されたトークンポインタの配列
 * @param num_tokens 配列内のトークンの数
 */
void free_split_tokens(char** tokens, int num_tokens) {
    if (tokens == NULL) {
        return;
    }
    for (int i = 0; i < num_tokens; i++) {
        free(tokens[i]); // 各トークン文字列を解放
    }
    free(tokens); // トークンポインタの配列自体を解放
}

int main() {
    const char* sentence1 = "  Hello   World! This is  a test. ";
    const char* sentence2 = "SingleWord";
    const char* sentence3 = "   leading and trailing spaces   ";
    const char* sentence4 = ""; // 空文字列
    const char* sentence5 = "  \t\n  "; // 全体が空白
    const char* sentence6 = "   \n \v   "; // 全体が空白

    int num_tokens;
    char** tokens;

    printf("--- Processing sentence 1 ---\n");
    tokens = split_by_whitespace(sentence1, &num_tokens);
    if (tokens) {
        printf("Original: \"%s\"\n", sentence1);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens);
    } else {
        printf("Original: \"%s\"\n", sentence1);
        printf("Result: NULL (no tokens)\n"); // 修正された出力
    }
    printf("\n");

    printf("--- Processing sentence 2 ---\n");
    tokens = split_by_whitespace(sentence2, &num_tokens);
    if (tokens) {
        printf("Original: \"%s\"\n", sentence2);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens);
    } else {
        printf("Original: \"%s\"\n", sentence2);
        printf("Result: NULL (no tokens)\n"); // 修正された出力
    }
    printf("\n");

    printf("--- Processing sentence 3 ---\n");
    tokens = split_by_whitespace(sentence3, &num_tokens);
    if (tokens) {
        printf("Original: \"%s\"\n", sentence3);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens);
    } else {
        printf("Original: \"%s\"\n", sentence3);
        printf("Result: NULL (no tokens)\n"); // 修正された出力
    }
    printf("\n");

    printf("--- Processing sentence 4 (empty string) ---\n");
    tokens = split_by_whitespace(sentence4, &num_tokens);
    if (tokens) {
        printf("Original: \"%s\"\n", sentence4);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens); 
    } else {
        printf("Original: \"%s\"\n", sentence4);
        printf("Result: NULL (no tokens)\n"); // 修正された出力
    }
    printf("\n");

    printf("--- Processing sentence 5 (all whitespace) ---\n");
    tokens = split_by_whitespace(sentence5, &num_tokens);
    if (tokens) { 
        // このブロックは実行されないはず。NULLが返されるため。
        printf("Original: \"%s\"\n", sentence5);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens);
    } else {
        printf("Original: \"%s\"\n", sentence5);
        printf("Result: NULL (no tokens)\n"); // このメッセージが表示されるはず
    }
    printf("\n");
    
    printf("--- Processing sentence 6 (all whitespace) ---\n");
    tokens = split_by_whitespace(sentence6, &num_tokens);
    if (tokens) { 
        // このブロックも実行されないはず。NULLが返されるため。
        printf("Original: \"%s\"\n", sentence6);
        printf("Tokens (%d):\n", num_tokens);
        for (int i = 0; i < num_tokens; i++) {
            printf("  [%d]: \"%s\"\n", i, tokens[i]);
        }
        free_split_tokens(tokens, num_tokens);
    } else {
        printf("Original: \"%s\"\n", sentence6);
        printf("Result: NULL (no tokens)\n"); // このメッセージが表示されるはず
    }
    printf("\n");

    return 0;
}