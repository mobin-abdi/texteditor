#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#define ESC 27
#define BACKSPACE 127

struct termios orig_termios;
char text[1024] = {0};  
int cursor = 0;
int command_mode = 0;   
char command[10] = {0}; 
int cmd_index = 0;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void saveToFile() {
    FILE *file = fopen("output.txt", "w");
    if (file) {
        fwrite(text, 1, cursor, file);
        fclose(file);
        printf("\n[File saved as output.txt]\n");
    } else {
        printf("\n[Error saving file!]\n");
    }
}

void handleCommand() {
    command[cmd_index] = '\0';  
    if (strcmp(command, ":wq") == 0) {
        saveToFile();
        printf("\n[Exiting...]\n");
        exit(0);
    } else {
        printf("\n[Unknown command: %s]\n", command);
    }
    cmd_index = 0;
    command_mode = 0;
}

int main()
{
    enableRawMode();
    printf("Vim-like Editor (ESC to Command Mode, :wq to save & exit)\n");

    int ch;
    while (1) {
        ch = getchar();

        if (command_mode) {
            if (ch == '\n') {
                handleCommand();
            } else if (ch == BACKSPACE && cmd_index > 0) { 
                cmd_index--;  
                printf("\b \b"); 
            } else {
                command[cmd_index++] = ch;
                putchar(ch);
            }
        } else {
            if (ch == ESC) {
                command_mode = 1;
                printf("\n:");
            } else if (ch == BACKSPACE && cursor > 0) { 
                cursor--;  
                printf("\b \b");
            } else {
                text[cursor++] = ch;
                putchar(ch);
            }
        }
    }
    return 0;
}