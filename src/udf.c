/** ===============================================================================
 Author______: Andrei Markus, Erni Feix Junior, Lucas Antonio Donida Rodrigues
 Date________: 06/04/2024
 Utility_____: Editor Hexadecimal
 Compiler cmd: gcc udf.c -o udf
 ============================================================================== **/

#include "udf.h"

#if defined(_WIN32)
#include <Windows.h>
#include <conio.h>
#else
#include <time.h>
#include "termios.h"
#include "unistd.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_TOP_OFFSET 2
#define DATA_LEFT_OFFSET 11
#define MAX_MENU 4

int max_pointer = 0;
int first_line = 0;

void clear() {
#if defined(_WIN32)
    color(color_white, color_black);
    system("cls");
#else
    printf("\x1b[0;0m");
    system("clear");
#endif
    gotoxy(0, 0);
}

#if !defined(_WIN32)
int getch(void) {
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
#endif

void drawbox(int x1, int y1, int x2, int y2) {
    int i;
    gotoxy(x1, y1);
    printf("%c", 218);
    for (i = x1; i < x2 - 1; i++) {
        printf("%c", 196);
    }
    printf("%c", 191);

    for (i = y1 + 1; i <= y2; i++) {
        gotoxy(x1, i);
        printf("%c", 179);
        spaces(x2 - x1 - 2);
        printf("%c", 179);
    }
    gotoxy(0, y2);
    printf("%c", 192);
    for (i = x1; i < x2 - 1; i++) {
        printf("%c", 196);
    }
    printf("%c", 217);
}

void fillbox(int x1, int y1, int x2, int y2) {
    int i;
    for (i = y1 + 1; i < y2; i++) {
        gotoxy(x1 + 1, i);
        spaces(x2 - x1 - 2);
    }
}

void textcolor(int color) {
#if defined(_WIN32)
    HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    BOOL b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
    bufferInfo.wAttributes &= 0x00F0;
    SetConsoleTextAttribute(hl, bufferInfo.wAttributes |= color);
#else
    printf("\x1b[%im", color);
#endif
}

void textbackground(int color) {
#if defined(_WIN32)
    HANDLE console_color;
    console_color = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console_color, color * 16);
#else
    printf("\x1b[%im", color + 10);
#endif
}

void color(int textColor, int backgroundColor) {
    textbackground(backgroundColor);
    textcolor(textColor);
}

void spaces(int s) {
    int i;
    for (i = 0; i <= s; i++) {
        printf(" ");
    }
}

void gotoxy(int x, int y) {
#if defined(_WIN32)
    COORD coord;
    coord.X = (short)x;
    coord.Y = (short)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

int stringlen(char text[]) {
    int c = 0;
    while (text[c] != 0) {
        c++;
    }
    return c;
}

void textcenter(int x1, int x2, int y1, char text[]) {
    int midtext = stringlen(text) / 2;
    int midwindow = x1 + (x2 - x1) / 2 - midtext;
    gotoxy(midwindow, y1);
    printf("%s", text);
}

void window(int x1, int y1, int x2, int y2, char text[]) {
    int i;

    color(color_white, color_dark_red);

    gotoxy(x1, y1);
    spaces(x2 - x1);
    textcenter(x1, x2, y1, text);

    color(color_black, color_cyan);
    drawbox(x1, y1 + 1, x2, y2);
    fillbox(x1, y1 + 1, x2, y2);
    color(color_black, color_cyan);
}

void get_data_slice(const struct data_array* data, int offset, int size,
                    unsigned char* out) {
    int i = 0;

    memset(out, 0, size);

    memset(out, 0, size);
    if (data->lenght - offset >= size) {
        memcpy(out, &(data->data[offset]), size);
    } else {
        memcpy(out, &(data->data[offset]), data->lenght - offset);
    }
}

void show_data(const struct data_array* file, int pointer, int* first_line) {

    int l;
    int c;
    int b;
    unsigned char data[ROWS * COLS];

    if ((pointer / COLS) >= (*first_line + ROWS)) {
        *first_line = pointer / COLS - ROWS + 1;
    }
    if (pointer / COLS < *first_line) {
        *first_line = pointer / COLS;
    }
    get_data_slice(file, *first_line * COLS, ROWS * COLS, data);

    // Desenha numero linha
    for (l = 0; l < ROWS; l++) {
        gotoxy(1, l + DATA_TOP_OFFSET);
        printf("%6d   ", (*first_line + l) * COLS);
    }

    for (l = 0; l < ROWS; l++) {
        gotoxy(DATA_LEFT_OFFSET, l + DATA_TOP_OFFSET);
        for (c = 0; c < COLS; c++) {
            if ((*first_line * COLS + l * COLS + c) == pointer) {
                color(color_white, color_dark_blue); // Destaque de seleção
            } else {
                color(color_black, color_cyan);
            }
            printf("%02X ", data[l * COLS + c]);
            color(color_black, color_cyan);
        }
        printf("   ");

        for (c = 0; c < COLS; c++) {
            if ((*first_line * COLS + l * COLS + c) == pointer) {
                color(color_white, color_dark_blue); // Destaque de seleção
            } else {
                color(color_black, color_cyan);
            }
            if (data[l * COLS + c] < 32 || data[l * COLS + c] > 126) {
                printf(".");
            } else {
                printf("%c", data[l * COLS + c]);
            }
            color(color_black, color_cyan);
        }
    }
#define BUFF_SIZE COLS * 4 + DATA_LEFT_OFFSET
    char buff[BUFF_SIZE];
    memset(buff, ' ', BUFF_SIZE);
    buff[BUFF_SIZE - 1] = '\0';
    show_menu(buff, 0);
    if (pointer < file->lenght) {
        if (pointer < file->lenght - 3) {
            sprintf(buff, "bin: 00000000\t8b int: %i\t32b int: %i\tfloat: %e",
                    *((char*)(file->data + pointer)),
                    *((int*)(file->data + pointer)),
                    *((float*)(file->data + pointer)));
        } else {
            sprintf(buff, "bin: 00000000\t8b int: %i",
                    *((char*)(file->data + pointer)));
        }
        for (int i = 0; i < 8; ++i) {
            if (file->data[pointer] & (1 << i)) {
                buff[4 + 8 - i] = '1';
            }
        }

        show_menu(buff, 0);
    }

    gotoxy(DATA_LEFT_OFFSET + (pointer % COLS) * 3,
           DATA_TOP_OFFSET + (pointer / COLS) - *first_line);
}

void show_menu(char* menu, int offset) {
    gotoxy(2, DATA_TOP_OFFSET + ROWS + offset);
    printf("%s", menu);
}

void show_editor() {
    clear();
    unsigned int x1, x2, y1, y2;
    x1 = 0;
    x2 = COLS * 4 + 16;
    y1 = 0;
    y2 = ROWS + 2 + MAX_MENU;

    window(x1, y1, x2, y2, "EDITOR HEXADECIMAL");
    show_menu("Q - Fechar\tWASD - Mover\tE - Editar\tI - Inserir\tX - Apagar",
              1);
    show_menu("F - Salvar\tG - Salvar como\tO - Abrir\tB - Buscar\tEsc - Sair",
              2);
    show_menu("H - Editar valor hexadecimal", 3);
}

void delay(int ms) {
#if defined(_WIN32)
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
#endif
}

int get_input() { return getch(); }

void move_pointer_top(const struct data_array* file, int* pointer) {
    if (*pointer >= COLS) {
        (*pointer) -= COLS;
    }
}

void move_pointer_left(const struct data_array* file, int* pointer) {
    if (*pointer > 0) {
        (*pointer)--;
    }
}

void move_pointer_right(const struct data_array* file, int* pointer) {
    if (*pointer < file->lenght) {
        (*pointer)++;
    }
}

void move_pointer_down(const struct data_array* file, int* pointer) {
    if ((*pointer + COLS) <= file->lenght) {
        (*pointer) += COLS;
    }
}

void grow_file(struct data_array* file) {
    unsigned char* c = malloc(file->capacity * 1.5 + 1);
    if (file->data) {
        memcpy(c, file->data, file->capacity);
        free(file->data);
    }
    file->data = c;
    file->capacity = file->capacity * 1.5 + 1;
}

void delete_at(struct data_array* file, int* pointer) {
    int i;
    struct data_array* c;
    if (*pointer > file->lenght) {
        return;
    }
    for (i = *pointer; i < file->lenght; i++) {
        file->data[i] = file->data[i + 1];
    }
    file->lenght--;
    move_pointer_left(file, pointer);
}

void insert_at(struct data_array* file, int* pointer, unsigned char c) {
    int i;
    while (*pointer >= file->capacity) {
        grow_file(file);
    }
    i = file->capacity - 1;
    while (i > *pointer) {
        file->data[i] = file->data[i - 1];
        i--;
    }
    file->data[*pointer] = c;
    file->lenght++;
    move_pointer_right(file, pointer);
}

void edit_hex_at(struct data_array* file, int* pointer) {
    unsigned char new_value = 0;
    int input;
    int digit_count = 0;
    int hex_digit;

    while (digit_count < 2) {
        input = getch();
        hex_digit = toupper(input);

        if (isxdigit(hex_digit)) {
            if (isdigit(hex_digit)) {
                hex_digit -= '0';
            } else {
                hex_digit = hex_digit - 'A' + 10;
            }

            if (digit_count == 0) {
                new_value = hex_digit << 4;
            } else {
                new_value |= hex_digit;
                file->data[*pointer] = new_value;
            }

            printf("%c", input); // Exibe o caractere digitado
            digit_count++;
        }
    }
}

void open_file(struct data_array* file, char* filename) {
    FILE* handle; // handle do arquivo

    clear();
    printf("Abrir arquivo:");
    fgets(filename, BUFFERSIZE, stdin);
    filename[strcspn(filename, "\r\n")] = 0;

    if ((handle = fopen(filename, "rb")) == NULL) { // conseguiu abrir ?
        printf("Não foi possivel abrir o arquivo %s", filename);
        delay(1500);
        show_editor();
        show_data(file, 0, &first_line);
        return;
    }

    // Carregar o arquivo
    fseek(handle, 0L, SEEK_END);
    file->lenght = ftell(handle);
    file->capacity = file->lenght * 1.5;
    file->data = malloc(file->capacity);
    if (file->data > 0) {
        rewind(handle);
        fread(file->data, 1, file->lenght, handle);
    }
    fclose(handle);
    show_editor();
}

void save_file(const struct data_array* file, const char* filename) {
    FILE* handle;
    if ((handle = fopen(filename, "wb")) == NULL) {
        printf("Não foi possivel salvar o arquivo %s", filename);
        delay(1000);
        return;
    }
    fwrite(file->data, 1, file->lenght, handle);
    fclose(handle);
}

void save_file_as(const struct data_array* file, char* filename) {
    clear();
    printf("Salvar arquivo como:");
    fgets(filename, BUFFERSIZE, stdin);
    filename[strcspn(filename, "\r\n")] = 0;
    save_file(file, filename);
    show_editor();
}

int search_data(const struct data_array* file, const char* search_str, int start_pos) {
    int str_len = strlen(search_str);
    int i = start_pos + 1; // Começa a busca a partir da próxima posição após a posição inicial

    while (i <= file->lenght - str_len) {
        if (memcmp(&file->data[i], search_str, str_len) == 0) {
            return i; // Retorna a posição
        }
        i++;
    }

    return -1; // Retorna -1 se não encontrar
}
