#ifndef SHELL_H
#define SHELL_H

/* 標準ライブラリのヘッダーファイル */
#include <stdio.h>      /* 標準入出力関数 */
#include <stdlib.h>     /* 一般ユーティリティ関数 */
#include <string.h>     /* 文字列操作関数 */
#include <unistd.h>     /* UNIX標準関数 */
#include <sys/types.h>  /* 基本システムデータ型 */
#include <sys/wait.h>   /* プロセス制御 */
#include <sys/stat.h>   /* ファイル状態 */
#include <fcntl.h>      /* ファイル制御 */
#include <signal.h>     /* シグナル処理 */
#include <errno.h>      /* エラー番号 */
#include <ctype.h>      /* 文字分類関数 */
#include <limits.h>     /* 実装の制限 */
#include <pwd.h>        /* パスワードファイルエントリ */
#include <grp.h>        /* グループファイルエントリ */
#include <dirent.h>     /* ディレクトリエントリ */
#include <time.h>       /* 時間関数 */

/* GNU Readline ライブラリ */
#include <readline/readline.h>
#include <readline/history.h>

// トークンの種類を定義するenum
typedef enum {
    T_WORD,         // コマンド名、引数、ファイル名など
    T_PIPE,         // |
    T_REDIR_IN,     // <
    T_REDIR_OUT,    // >
    T_REDIR_APPEND, // >>
    T_HEREDOC,      // <<
    T_EOF           // 入力の終わり
} TokenType;

// トークン構造体
typedef struct Token {
    char *value;
    TokenType type;
    struct Token *next;
} Token;

/*コマンドパーサー*/
typedef struct Command {
    char **argv;          // コマンドと引数の配列 (例: {"ls", "-l", NULL})
    char *redirect_in;    // 入力リダイレクトのファイル名
    char *redirect_out;   // 出力リダイレクトのファイル名
	TokenType append_mode; // >>かどうか判別
	char *heredoc_delimiter;
    struct Command *next; // パイプで繋がる次のコマンド
} Command;

/* マクロ定義 */
#define MAX_LINE 80     /* コマンドラインの最大長 */
#define MAX_ARGS 64     /* 引数の最大数 */
#define MAX_PATH 1024   /* パスの最大長 */

/* 関数宣言 */
char** split_by_whitespace(const char* str, size_t* num_tokens);
void free_split_tokens(char** tokens, size_t num_tokens);
void print_prompt(void);
char *read_line(void);
char **parse_line(char *line);
int execute_command(char **args);
void handle_signal(int sig);
void shell_animation(void);
Command* create_command_node(void);
void free_command(Command* cmd);
void appendCommand(struct Command** head, char **argv, char *redirect_in, char *redirect_out);
void free_command_list(Command* head);
const char* token_type_to_string(TokenType type);
Token* create_token_node(char* str, TokenType type);
void free_token_list(Token* head);
Token* tokenize_strings(char** str_array, size_t count);
Command* parser(char* line);
void print_command_list(Command* head);

#endif /* SHELL_H */
