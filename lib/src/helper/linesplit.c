#include <shell.h>

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
char** split_by_whitespace(const char* str, size_t* num_tokens) {
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
