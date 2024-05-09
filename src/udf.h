#ifndef UDF_HEADER
#define UDF_HEADER

enum colors {
    color_black = 0,
    color_dark_blue = 1,
    color_dark_green = 2,
    color_light_blue = 3,
    color_dark_red = 4,
    color_magenta = 5,
    color_orange = 6,
    color_light_gray = 7,
    color_gray = 8,
    color_blue = 9,
    color_green = 10,
    color_cyan = 11,
    color_red = 12,
    color_pink = 13,
    color_yellow = 14,
    color_white = 15
};

void clear();
void drawbox(int x1, int y1, int x2, int y2);
void textcolor(int color);
void fillbox(int x1, int y1, int x2, int y2);
void textbackground(int color);
void color(int tC, int bC);
void spaces(int s);
void gotoxy(int x, int y);
int stringlen(char text[]);
void textcenter(int x1, int x2, int y1, char text[]);
void window(int x1, int y1, int x2, int y2, char text[]);
void display_data(int cols, int rows, char* data, int line);
void delay(int t);
int achoice(int x, int y, int x1, int y1, int numopcoes, char opcoes[][20]);

#endif
