
int kbhit(void);
int getch(void);

void gotoxy(int x, int y);
void delay(int seconds); // delay (tham so truyen vao la so ms delay)
char *readFile(char *filename);

void clearRect(int start_x, int start_y, int end_x, int end_y); // xoa 1 khu vuc theo hinh chu nhat
void clearRectReverse(int start_x, int start_y, int width, int height);
void clearScreen();                                             // xoa man hinh
void drawBorder();                                              // ve vien
void drawSelectMenu();                                          // ve select menu
void testDraw();
void drawStand();

void drawMainMenu(); // Ve man hinh khoi dong
void drawLoginPage(int sockfd); // Ve man dang nhap
void drawSignUpPage(int sockfd); // Ve man dang ky
void drawReadyPage(); // Ve man play & rank (page sau khi login thanh cong)
void drawRank();
void drawCommentPane();

void returnMenu();

void quit();