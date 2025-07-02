CC = gcc
CFLAGS = -Wall -Wextra -I./lib/include
LDFLAGS = -lreadline
SRCDIR = app
LIBDIR = lib/src
HELPERDIR = lib/src/helper
OBJDIR = obj
BINDIR = bin

# ソースファイル
SOURCES = $(wildcard $(SRCDIR)/*.c)
LIBSOURCES = $(wildcard $(LIBDIR)/*.c)
HELPERSOURCES = $(wildcard $(HELPERDIR)/*.c)

# オブジェクトファイル
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
LIBOBJECTS = $(LIBSOURCES:$(LIBDIR)/%.c=$(OBJDIR)/%.o)
HELPEROBJECTS = $(HELPERSOURCES:$(HELPERDIR)/%.c=$(OBJDIR)/%.o)

# ターゲット
TARGET = $(BINDIR)/myshell

# デフォルトターゲット
all: $(TARGET)

# 実行ファイルの作成
$(TARGET): $(OBJECTS) $(LIBOBJECTS) $(HELPEROBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) $(LIBOBJECTS) $(HELPEROBJECTS) -o $(TARGET) $(LDFLAGS)

# アプリケーションのオブジェクトファイルの作成
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ライブラリのオブジェクトファイルの作成
$(OBJDIR)/%.o: $(LIBDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ヘルパーライブラリのオブジェクトファイルの作成
$(OBJDIR)/%.o: $(HELPERDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ディレクトリの作成
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# クリーンアップ
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# デバッグ用ターゲット
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# インストール（オプション）
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean debug install 