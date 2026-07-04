#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
void count_lines(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("");
        return;
    }

    int total_lines = 0;
    int blank_lines = 0;
    int comment_lines = 0;
    char line[4096];
    int in_block_comment = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        total_lines++;

        int is_blank = 1;
        int i;
        for (i = 0; line[i] != '\0'; i++) {
            char c = line[i];
            if (c == '\r') continue;
            if (c != ' ' && c != '\t' && c != '\n') {
                is_blank = 0;
                break;
            }
        }

        if (is_blank) {
            blank_lines++;
            continue;
        }

        int is_comment = 0;
        int in_string = 0;
        
        for (i = 0; line[i] != '\0'; i++) {
            char c = line[i];
            if (c == '\r' || c == '\n') break;

            if (c == '"' && !in_string) {
                int j = i - 1, count = 0;
                while (j >= 0 && line[j] == '\\') {
                    count++;
                    j--;
                }
                if (count % 2 == 0) {
                    in_string = 1;
                }
                continue;
            } else if (c == '"' && in_string) {
                int j = i - 1, count = 0;
                while (j >= 0 && line[j] == '\\') {
                    count++;
                    j--;
                }
                if (count % 2 == 0) {
                    in_string = 0;
                }
                continue;
            }

            if (in_string) continue;

            if (in_block_comment) {
                if (c == '*' && line[i + 1] != '\0' && line[i + 1] == '/') {
                    in_block_comment = 0;
                    i++;
                }
                is_comment = 1;
            } else {
                if (c == '/' && line[i + 1] != '\0' && line[i + 1] == '/') {
                    is_comment = 1;
                    break;
                }
                if (c == '/' && line[i + 1] != '\0' && line[i + 1] == '*') {
                    in_block_comment = 1;
                    is_comment = 1;
                    i++;
                    continue;
                }
                if (c != ' ' && c != '\t') {
                    break;
                }
            }
        }

        if (is_comment) {
            comment_lines++;
        }
    }

    fclose(fp);

    printf("总行数: %d\n", total_lines);
    printf("空行数: %d\n", blank_lines);
    printf("注释行数: %d\n", comment_lines);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    if (argc < 2) {
        printf("用法: main.exe <文件名>\n");
        return 1;
    }

    count_lines(argv[1]);
    return 0;
}