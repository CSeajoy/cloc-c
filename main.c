#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
void count_lines(const char *filename, int *total, int *blank, int *comment) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
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

    *total = total_lines;
    *blank = blank_lines;
    *comment = comment_lines;
}

void scan_directory(const char *path, int *total_files, int *total_lines, int *total_blank, int *total_comment) {
    char search_path[4096];
    snprintf(search_path, sizeof(search_path), "%s\\*.*", path);
    
#ifdef _WIN32
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }
    
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) {
            continue;
        }
        
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s\\%s", path, fd.cFileName);
        
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            scan_directory(full_path, total_files, total_lines, total_blank, total_comment);
        } else {
            const char *ext = strrchr(fd.cFileName, '.');
            if (ext && (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0)) {
                int t, b, c;
                count_lines(full_path, &t, &b, &c);
                printf("%s: 总行数: %d, 空行数: %d, 注释行数: %d\n", full_path, t, b, c);
                (*total_files)++;
                *total_lines += t;
                *total_blank += b;
                *total_comment += c;
            }
        }
    } while (FindNextFile(hFind, &fd));
    
    FindClose(hFind);
#endif
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    if (argc == 1) {
        int total_files = 0;
        int total_lines = 0;
        int total_blank = 0;
        int total_comment = 0;
        scan_directory(".", &total_files, &total_lines, &total_blank, &total_comment);
        printf("\n汇总: 文件数: %d, 总行数: %d, 总空行数: %d, 总注释行数: %d\n", 
               total_files, total_lines, total_blank, total_comment);
        return 0;
    }
    
    if (strcmp(argv[1], "-path") == 0) {
        if (argc < 3) {
            printf("用法: main.exe -path <目录>\n");
            return 1;
        }
        
        int total_files = 0;
        int total_lines = 0;
        int total_blank = 0;
        int total_comment = 0;
        
        scan_directory(argv[2], &total_files, &total_lines, &total_blank, &total_comment);
        
        printf("\n汇总: 文件数: %d, 总行数: %d, 总空行数: %d, 总注释行数: %d\n", 
               total_files, total_lines, total_blank, total_comment);
    } else {
        int t, b, c;
        count_lines(argv[1], &t, &b, &c);
        printf("总行数: %d\n", t);
        printf("空行数: %d\n", b);
        printf("注释行数: %d\n", c);
    }
    
    return 0;
}