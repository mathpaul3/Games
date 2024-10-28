/*
- compile
WIN: gcc tetris.c -o tetris
MAC: gcc tetris.c -o tetris -lncurses
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 테트리스가 돌아갈 운영체제
// #define __APPLE__
// #define _WIN32

// 편의를 위한 자료형과 상수
#define bool char
#define true 1
#define false 0

// 테트리미노의 개수
#define TETRIMINO_NUM 7
#define TETRIMINO_START_X 4
#define TETRIMINO_START_Y 4
#define ROTATION_NUM 4

// 필드의 크기와 위치
#define FIELD_COL 10
#define FIELD_ROW 20
#define FIELD_ROW_OFFSET 4
#define FIELD_X HOLD_X+5*2
#define FIELD_Y 2

// 타이틀, 게임오버 메시지의 위치
#define TITLE_X 5
#define TITLE_Y 4
#define GAMEOVER_X FIELD_X
#define GAMEOVER_Y FIELD_Y+(int)(FIELD_ROW/2)-3

// 홀드의 위치
#define HOLD_X 2
#define HOLD_Y FIELD_Y+2

// 넥스트의 위치와 간격, 보일 넥스트의 개수
#define NXT_X FIELD_X+(FIELD_COL*2+2)+4
#define NXT_Y FIELD_Y+3
#define NXT_INTERVAL 3
#define NXT_SHOW 5

// 블럭 색상
#define BLACK_PAIR 1
#define RED_PAIR 2
#define GREEN_PAIR 3
#define YELLOW_PAIR 4
#define BLUE_PAIR 5
#define MAGENTA_PAIR 6
#define CYAN_PAIR 7
#define WHITE_PAIR 8

// 추가 기능
#define SPEED 10 // (int)1ms/SPEED
#define LOCKDOWN_DELAY_TIME (int)(1000/SPEED)*2 //
#define LOCKDOWN_DELAY_LIMIT 15

// 좌표 표현
typedef struct COORDINATE {
    int x;
    int y;
} COORDINATE;

// 테트리미노
typedef struct TETRIMINO {
    COORDINATE *center;
    int type;
    int rotation;
    int colors[4];
} TETRIMINO;

// 테트리미노 타입
typedef enum {I_MINO, J_MINO, L_MINO, O_MINO, S_MINO, Z_MINO, T_MINO} MINO_TYPE;

// 회전축을 기준으로 [테트리미노 타입][회전][각 cell의 상대적 위치]
COORDINATE tetrimino[TETRIMINO_NUM][4][4]={
    {   // I미노
        {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},     // 정방향 가로
        {{1, -1}, {1, 0}, {1, 1}, {1, 2}},     // 정방향 세로
        {{2, 1}, {1, 1}, {0, 1}, {-1, 1}},     // 역방향 가로
        {{0, 2}, {0, 1}, {0, 0}, {0, -1}},     // 역방향 세로
    },
    {   // J미노
        {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}},    // 정방향 가로
        {{1, -1}, {0, -1}, {0, 0}, {0, 1}},     // 정방향 세로
        {{1, 1}, {1, 0}, {0, 0}, {-1, 0}},      // 역방향 가로
        {{-1, 1}, {0, 1}, {0, 0}, {0, -1}},     // 역방향 세로
    },
    {   // L미노
        {{1, -1}, {-1, 0}, {0, 0}, {1, 0}},     // 정방향 가로
        {{1, 1}, {0, -1}, {0, 0}, {0, 1}},      // 정방향 세로
        {{-1, 1}, {1, 0}, {0, 0}, {-1, 0}},     // 역방향 가로
        {{-1, -1}, {0, 1}, {0, 0}, {0, -1}},    // 역방향 세로
    },
    {   // O미노
        {{0, -1}, {1, -1}, {0, 0}, {1, 0}},     // 정방향 가로
        {{1, -1}, {1, 0}, {0, -1}, {0, 0}},     // 정방향 세로
        {{1, 0}, {0, 0}, {1, -1}, {0, -1}},     // 역방향 가로
        {{0, 0}, {0, -1}, {1, 0}, {1, -1}},     // 역방향 세로
    },
    {   // S미노
        {{0, -1}, {1, -1}, {-1, 0}, {0, 0}},    // 정방향 가로
        {{1, 0}, {1, 1}, {0, -1}, {0, 0}},      // 정방향 세로
        {{0, 1}, {-1, 1}, {1, 0}, {0, 0}},      // 역방향 가로
        {{-1, 0}, {-1, -1}, {0, 1}, {0, 0}},    // 역방향 세로
    },
    {   // Z미노
        {{-1, -1}, {0, -1}, {0, 0}, {1, 0}},    // 정방향 가로
        {{1, -1}, {1, 0}, {0, 0}, {0, 1}},      // 정방향 세로
        {{1, 1}, {0, 1}, {0, 0}, {-1, 0}},      // 역방향 가로
        {{-1, 1}, {-1, 0}, {0, 0}, {0, -1}},    // 역방향 세로
    },
    {   // T미노
        {{0, -1}, {-1, 0}, {0, 0}, {1, 0}},     // 정방향 가로
        {{1, 0}, {0, -1}, {0, 0}, {0, 1}},      // 정방향 세로
        {{0, 1}, {1, 0}, {0, 0}, {-1, 0}},      // 역방향 가로
        {{-1, 0}, {0, 1}, {0, 0}, {0, -1}},     // 역방향 세로
    },
};

/****************************************/
//   운영체제에 종속적인 함수는 매크로로 처리함    //
// 아래 목록중 주석 처리된 함수는 OS종속적인 함수  //
/****************************************/

/***********/
/* 게임 관련 */
/***********/
// 게임 초기화
void reset();
// 키보드 입력
int check_key();
// 게임 오버 여부 확인
int check_game_over(TETRIMINO *cur_mino);
// 타이틀 화면 보이기
void _show_title();
// 타이틀 화면 지우기
void _clear_title();
// 게임 오버 메시지 띄우기
void _show_game_over();
// 게임 오버 메시지 지우기
void _clear_game_over();

// Sleep(time): 밀리초 단위로 프로세스 일시정지
// MAC - usleep(time*1000)
// WIN - Sleep(time)

// kbhit(): 키보드 입력이 존재하는지 여부 확인
// MAC - termios.h 헤더를 활용한 사용자 정의 함수
// WIN - window.h 헤더에 포함된 kbhit()

// readch(): 키보드 입력이 있었다면 버퍼에서 1byte씩 읽어옴
// MAC - termios.h 헤더를 활용한 사용자 정의 함수
// WIN - window.h 헤더에 포함된 getch()

/***************/
/* 화면 출력 관련 */
/***************/
// 화면 출력에 색상 입히기 시작
void colorize(int type);
// 화면 출력에 색상 입히기 종료
void decolorize(int type);
// void gotoxy(Y,X): 화면의 특정 위치로 커서를 이동하기
// MAC - ncurses.h 헤더에 포함된 wmove(stdscr,Y,X)
// WIN - window.h 헤더를 활용한 사용자 정의 함수

// void printw(): 화면에 출력하기
// MAC - ncurses.h 헤더에 포함된 printw()
// WIN - printf()

// clr(): 화면 비우기
// MAC - system("clear")
// WIN - system("cls")

// CHNG(args...): 화면 갱신이 필요한 함수들
// MAC - _##args...; refresh()
// WIN - _##args...  별도의 조치 필요 없음


/***********/
/* 필드 관련 */
/***********/
// 필드 보이기
void _show_field(TETRIMINO *prv_mino, TETRIMINO *cur_mino, TETRIMINO *ghost_mino);
// 필드 지우기
void _clear_field();
// 클리어 확인하고 줄 내리기
void check_clear();

/************/
/* 고스트 관련 */
/************/
// 고스트 위치 설정
void set_ghost(TETRIMINO *cur_mino, TETRIMINO *ghost_mino);

/************/
/* 넥스트 관련 */
/************/
// 넥스트 배열 섞기 [앞 7개 / 뒤 7개]
void shuffle(int is_back);
// 넥스트 보이기
void _show_nxt();
// 넥스트 지우기
void _clear_nxt();
// 넥스트의 인덱스 증가
void increase_nxt(int *cur_nxt);

/***********/
/* 홀드 관련 */
/***********/
// 홀드하기
void hold(TETRIMINO *hold_mino, TETRIMINO *cur_mino);
// 홀드 보이기
void _show_hold(int prv_type, TETRIMINO *cur_type);
// 홀드 지우기
void _clear_hold(int cur_type);

/*******************/
/* 테트리미노 관련 기능 */
/*******************/
// 필드에서 테트리미노를 제거
void unset_tetrimino(TETRIMINO *mino);
// 필드에 테트리미노를 추가
void set_tetrimino(TETRIMINO *mino);
// 테트리미노 이동
int move_tetrimino(TETRIMINO *mino, int dir);
// 테트리미노 회전
int rotate_tetrimino(TETRIMINO *mino, int dir);
// 유효한 움직임인지 확인
int is_valid_move(TETRIMINO *mino, int dir);
// 바닥이나 다른 블럭에 닿아 블럭이 놓인 경우
void crashed(TETRIMINO *prv_mino, TETRIMINO *cur_mino, TETRIMINO *ghost_mino);
// 테트리미노에 색 입히기
void colorize_tetrimino(TETRIMINO *cur_mino);

#ifdef __APPLE__
    #include <ncurses.h>
    #include <termios.h>
    #include <locale.h>
    #include <unistd.h>
    #define gotoxy(Y,X) wmove(stdscr,Y,X)
    #define CHNG(args...) _##args; refresh()
    #define clr() system("clear")
    #define Sleep(time) usleep(time*1000)
    
    // 키보드 입력
    #define LEFT 68 // 좌로 이동
    #define RIGHT 67 // 우로 이동 
    #define UP 65 // 회전
    #define REV_UP 122 // 역회전
    #define UPSIDE_DOWN 97 // 180도 회전
    #define DOWN 66 // soft drop
    #define SPACE 32 // hard drop
    #define ENTER 13 // 엔터
    #define HOLD 99 // 홀드
    #define p 112 // 뿌요뿌요 모드
    #define CTRLC 3 // 게임 강제 종료
    #define q 113 // 메인화면으로 나가기

    static struct termios initial_settings, new_settings;
    static int peek_character = -1;
    int kbhit() {
        char ch;
        int nread;

        if(peek_character != -1)
            return 1;
        new_settings.c_cc[VMIN]=0;
        tcsetattr(0, TCSANOW, &new_settings);
        nread = read(0,&ch,1);
        new_settings.c_cc[VMIN]=1;
        tcsetattr(0, TCSANOW, &new_settings);

        if(nread == 1) {
            peek_character = ch;
            return 1;
        }
        return 0;
    }

    int readch() {
        char ch;

        if(peek_character != -1) {
            ch = peek_character;
            peek_character = -1;
            return ch;
        }
        read(0,&ch,1);
        return ch;
    }
#endif
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    
    #define printw(args...) printf(args)
    #define CHNG(args...) _##args
    #define clr() system("cls")
    #define readch() getch()

    // 키보드 입력
    #define LEFT 75 // 좌로 이동
    #define RIGHT 77 // 우로 이동 
    #define UP 72 // 회전
    #define REV_UP 122 // 역회전
    #define UPSIDE_DOWN 97 // 180도 회전
    #define DOWN 80 // soft drop
    #define SPACE 32 // hard drop
    #define ENTER 13 // 엔터
    #define HOLD 99 // 홀드
    #define p 112 // 일시정지
    #define CTRLC 3 // 게임 강제 종료
    #define q 113 // 메인화면으로 나가기

    void gotoxy(int y, int x) {
        COORD pos={x,y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
#endif

bool visited[FIELD_ROW+FIELD_ROW_OFFSET][FIELD_COL];
int pull_down[FIELD_ROW+FIELD_ROW_OFFSET][FIELD_COL];
int prv_field[FIELD_ROW+FIELD_ROW_OFFSET][FIELD_COL]={0,}, cur_field[FIELD_ROW+FIELD_ROW_OFFSET][FIELD_COL]={0,};
TETRIMINO nxt[TETRIMINO_NUM*2]; // 앞 쪽 7-bag, 뒷 쪽 7-bag
int cur_nxt=1, key;
int milisec=0, lockdown_delay=LOCKDOWN_DELAY_TIME+1, lockdown_delay_cnt=0;
bool is_chaining=false, already_held=false, is_placed=false, game_over=false;
int mv[4]={0,0,-1,1};

int main() {
    srand(time(NULL));
    clr();
    #ifdef __APPLE__
        setlocale(LC_ALL, "");
        initscr();

        start_color();
        init_pair(BLACK_PAIR, COLOR_BLACK, COLOR_BLACK);
        init_pair(RED_PAIR, COLOR_RED, COLOR_BLACK);
        init_pair(GREEN_PAIR, COLOR_GREEN, COLOR_BLACK);
        init_pair(YELLOW_PAIR, COLOR_YELLOW, COLOR_BLACK);
        init_pair(BLUE_PAIR, COLOR_BLUE, COLOR_BLACK);
        init_pair(MAGENTA_PAIR, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(CYAN_PAIR, COLOR_CYAN, COLOR_BLACK);
        init_pair(WHITE_PAIR, COLOR_WHITE, COLOR_BLACK);
    #endif

    while (1) {
        CHNG(show_title());
        Sleep(100);
        if (!kbhit()) continue;
        key = check_key();
        CHNG(clear_title());
        if (key==q) {clr(); return 0; }

        reset();

        COORDINATE prv_center={TETRIMINO_START_X, TETRIMINO_START_Y},
        cur_center={TETRIMINO_START_X, TETRIMINO_START_Y},
        ghost_center={TETRIMINO_START_X, TETRIMINO_START_Y};
        TETRIMINO prv_mino={&prv_center, nxt[cur_nxt-1].type, 0, {0,0,0,0}},
        cur_mino={&cur_center, nxt[cur_nxt-1].type, 0, {0,0,0,0}},
        ghost_mino={&ghost_center, nxt[cur_nxt-1].type, 0, {0,0,0,0}},
        cur_hold={0, -1, 0, {0,0,0,0}};
        

        for (int cell=0; cell<4; cell++) {
            prv_mino.colors[cell] = nxt[cur_nxt-1].colors[cell];
            cur_mino.colors[cell] = nxt[cur_nxt-1].colors[cell];
        }
        set_tetrimino(&cur_mino);
        CHNG(show_field(&prv_mino, &cur_mino, &ghost_mino));
        CHNG(show_nxt());

        while (1) {
            if (game_over) {
                if (!kbhit()) {
                    CHNG(show_game_over());
                    Sleep(100);
                    continue;
                }
                key = check_key();
                if (key!=ENTER) continue;
                CHNG(clear_field());
                CHNG(clear_hold(cur_hold.type));
                CHNG(clear_nxt());
                CHNG(clear_game_over());
                break;
            }
            if (kbhit()) {
                key = check_key();
                if (key==LEFT || key==RIGHT || key==DOWN || key==SPACE)
                    move_tetrimino(&cur_mino, key);
                else if (key==UP || key==REV_UP || key==UPSIDE_DOWN) {
                    rotate_tetrimino(&cur_mino, key);
                    lockdown_delay=0;
                }
                else if (key==HOLD)
                    hold(&cur_hold, &cur_mino);
                CHNG(show_field(&prv_mino, &cur_mino, &ghost_mino));
                if (key==SPACE) {
                    crashed(&prv_mino, &cur_mino, &ghost_mino);
                    CHNG(show_field(&prv_mino, &cur_mino, &ghost_mino));
                }
                CHNG(show_nxt());
            } else if (milisec>(int)(1000/SPEED)) {
                if (!move_tetrimino(&cur_mino, DOWN)) {
                    if (lockdown_delay<=LOCKDOWN_DELAY_TIME && lockdown_delay_cnt<LOCKDOWN_DELAY_LIMIT) {
                        lockdown_delay_cnt++;
                        milisec = 0;
                    }
                    else if (!lockdown_delay_cnt || lockdown_delay>=LOCKDOWN_DELAY_TIME) {
                        crashed(&prv_mino, &cur_mino, &ghost_mino);
                        lockdown_delay_cnt = 0;
                        milisec = 0;
                    }
                    lockdown_delay = LOCKDOWN_DELAY_TIME+1;
                } else {
                    milisec = 0;
                    lockdown_delay = LOCKDOWN_DELAY_TIME+1;
                    lockdown_delay_cnt = 0;
                }
                CHNG(show_field(&prv_mino, &cur_mino, &ghost_mino));
                CHNG(show_nxt());
            }
            milisec++;
            lockdown_delay++;
            Sleep(10);
        }
    }

    #ifdef __APPLE__
        endwin();
    #endif
    clr();

    return 0;
}

void reset() {
    cur_nxt=1;
    milisec = 0, lockdown_delay=LOCKDOWN_DELAY_LIMIT+1, lockdown_delay_cnt=0;
    already_held=false, is_placed=false, game_over=false;

    int i, j, cell;
    for (i=0; i<FIELD_ROW+FIELD_ROW_OFFSET; i++) {
        if (i<FIELD_ROW) {
            gotoxy(FIELD_Y+i, FIELD_X-2);
            printw("■");
            gotoxy(FIELD_Y+i, FIELD_X+FIELD_COL*2);
            printw("■");
        } else if (i==FIELD_ROW) {
            for (j=-1; j<=FIELD_COL; j++) {
                gotoxy(FIELD_Y+i, FIELD_X+j*2);
                printw("■");
            }
        }
        for (j=0; j<FIELD_COL; j++) {
            prv_field[i][j] = -1;
            cur_field[i][j] = 0;
        }
    }
    for (i=0; i<TETRIMINO_NUM*2; i++) {
        nxt[i].type = i%TETRIMINO_NUM;
        colorize_tetrimino(&nxt[i]);
    }
    
    shuffle(false);
    shuffle(true);
}

int check_key() {
    int key = 0;
    if (kbhit()) {
        key = readch();
        while (key==27 || key==91)
            key = readch();
        if (game_over) return key;
        gotoxy(0,0);
        printw("%3d", key);
        if (key==CTRLC) {
            CHNG(clear_title());
            CHNG(clear_field());
            CHNG(clear_nxt());
            CHNG(clear_game_over());
            clr();
            exit(0);
        }
    }
    return key;
}

int check_game_over(TETRIMINO *cur_mino) {
    int idx;
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[cur_mino->type][cur_mino->rotation][idx];
        if (cur_field[cur_mino->center->y+cell.y][cur_mino->center->x+cell.x]) return true;
    }
    return false;
}

void _show_title() {
    gotoxy(TITLE_Y+0,TITLE_X);  printw("■□□□■■■□□■■□□■■□■■□■■");Sleep(100);
    gotoxy(TITLE_Y+1,TITLE_X);  printw("■■■□    P U Y O   □□■");Sleep(100);
    gotoxy(TITLE_Y+2,TITLE_X);  printw("□□□■  T E T R I S □■■");Sleep(100);
    gotoxy(TITLE_Y+3,TITLE_X);  printw("■■□■■■□□■    ■□■□□■□□");Sleep(100);
    gotoxy(TITLE_Y+4,TITLE_X);  printw("■■□■□□■□■    ■□□■□■■□");Sleep(100);
    gotoxy(TITLE_Y+5,TITLE_X);  printw("■■□■■■□□■■■□□■□■□□■□■");Sleep(100);
    gotoxy(TITLE_Y+7,TITLE_X);  printw("Please Enter Any Key to Start..");
    gotoxy(TITLE_Y+8,TITLE_X);  printw("                               ");
    gotoxy(TITLE_Y+10,TITLE_X); printw("   C   : Hold");
    gotoxy(TITLE_Y+11,TITLE_X); printw("   A   : Rotate 180˚");
    gotoxy(TITLE_Y+12,TITLE_X); printw("   Z   : Rotate Counterclockwise");
    gotoxy(TITLE_Y+13,TITLE_X); printw("   △   : Rotate Clockwise");
    gotoxy(TITLE_Y+14,TITLE_X); printw(" ◁   ▷ : Arrow Left / Arrow Right");
    gotoxy(TITLE_Y+15,TITLE_X); printw("   ▽   : Soft Drop");
    gotoxy(TITLE_Y+16,TITLE_X); printw(" SPACE : Hard Drop");
    gotoxy(TITLE_Y+18,TITLE_X); printw("   Q   : Quit");
}

void _clear_title() {
    gotoxy(TITLE_Y+0,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+1,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+2,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+3,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+4,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+5,TITLE_X);  printw("                     ");Sleep(100);
    gotoxy(TITLE_Y+7,TITLE_X);  printw("                               ");
    gotoxy(TITLE_Y+8,TITLE_X);  printw("                               ");
    gotoxy(TITLE_Y+10,TITLE_X); printw("             ");
    gotoxy(TITLE_Y+11,TITLE_X); printw("                    ");
    gotoxy(TITLE_Y+12,TITLE_X); printw("                                ");
    gotoxy(TITLE_Y+13,TITLE_X); printw("                         ");
    gotoxy(TITLE_Y+14,TITLE_X); printw("                                 ");
    gotoxy(TITLE_Y+15,TITLE_X); printw("                  ");
    gotoxy(TITLE_Y+16,TITLE_X); printw("                  ");
    gotoxy(TITLE_Y+18,TITLE_X); printw("             ");
}

void _show_game_over() {
    gotoxy(GAMEOVER_Y,GAMEOVER_X);   printw("■■■■■■■■■■■■■■■■■■■");
    gotoxy(GAMEOVER_Y+1,GAMEOVER_X); printw("■ G A M E O V E R ■");
    gotoxy(GAMEOVER_Y+2,GAMEOVER_X); printw("■                 ■");
    gotoxy(GAMEOVER_Y+3,GAMEOVER_X); printw("■   PRESS ENTER   ■");
    gotoxy(GAMEOVER_Y+4,GAMEOVER_X); printw("■    TO RETURN    ■");
    gotoxy(GAMEOVER_Y+5,GAMEOVER_X); printw("■■■■■■■■■■■■■■■■■■■");
}

void _clear_game_over() {
    gotoxy(GAMEOVER_Y,GAMEOVER_X);   printw("                   ");
    gotoxy(GAMEOVER_Y+1,GAMEOVER_X); printw("                   ");
    gotoxy(GAMEOVER_Y+2,GAMEOVER_X); printw("                   ");
    gotoxy(GAMEOVER_Y+3,GAMEOVER_X); printw("                   ");
    gotoxy(GAMEOVER_Y+4,GAMEOVER_X); printw("                   ");
    gotoxy(GAMEOVER_Y+5,GAMEOVER_X); printw("                   ");
}

void _show_field(TETRIMINO *prv_mino, TETRIMINO *cur_mino, TETRIMINO *ghost_mino) {
    int i, j, idx, y, x;
    gotoxy(1,0);
    printw("%2d", lockdown_delay_cnt);
    unset_tetrimino(prv_mino);
    game_over = check_game_over(cur_mino);
    if (game_over) return;
    set_tetrimino(cur_mino);
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[ghost_mino->type][ghost_mino->rotation][idx];
        y = ghost_mino->center->y+cell.y, x = ghost_mino->center->x+cell.x;
        if (cur_field[y][x]) continue;
        gotoxy(FIELD_Y+y-FIELD_ROW_OFFSET, FIELD_X+x*2);
        printw(" ");
    }
    set_ghost(cur_mino, ghost_mino);
    for (i=FIELD_ROW_OFFSET; i<FIELD_ROW+FIELD_ROW_OFFSET; i++) {
        for (j=0; j<FIELD_COL; j++) {
            if (prv_field[i][j]!=cur_field[i][j]) {
                gotoxy(FIELD_Y+i-FIELD_ROW_OFFSET, FIELD_X+j*2);
                if (cur_field[i][j]) colorize(cur_field[i][j]);
                printw("%s", (cur_field[i][j] ? "■" : " "));
                if (cur_field[i][j]) decolorize(cur_field[i][j]);
            }
            prv_field[i][j] = cur_field[i][j];
        }
    }
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[ghost_mino->type][ghost_mino->rotation][idx];
        y = ghost_mino->center->y+cell.y, x = ghost_mino->center->x+cell.x;
        if (cur_field[y][x]) continue;
        gotoxy(FIELD_Y+y-FIELD_ROW_OFFSET, FIELD_X+x*2);
        printw("□");
    }
    prv_mino->center->x = cur_mino->center->x;
    prv_mino->center->y = cur_mino->center->y;
    prv_mino->type = cur_mino->type;
    prv_mino->rotation = cur_mino->rotation;
}

void _clear_field() {
    int i, j;
    for (i=0; i<FIELD_ROW; i++) {
        gotoxy(FIELD_Y+i, FIELD_X-2);
        printw(" ");
        gotoxy(FIELD_Y+i, FIELD_X+FIELD_COL*2);
        printw(" ");

        for (j=0; j<FIELD_COL; j++) {
            gotoxy(FIELD_Y+i, FIELD_X+j*2);
            printw(" ");
        }
    }
    for (j=-1; j<=FIELD_COL; j++) {
        gotoxy(FIELD_Y+FIELD_ROW, FIELD_X+j*2);
        printw(" ");
    }
}

void check_clear() {
    int row, col, top_row=FIELD_ROW+FIELD_ROW_OFFSET;
    int is_clear[FIELD_ROW+FIELD_ROW_OFFSET]={0,};

    bool is_clear_start = false;
    for (row=FIELD_ROW+FIELD_ROW_OFFSET-1; row>=0; row--) {
        bool row_full = true;
        for (col=0; col<FIELD_COL; col++) {
            if (cur_field[row][col]==0)
                row_full = false;
            else top_row = row;
        }
        if (row != (FIELD_ROW+FIELD_ROW_OFFSET-1))
            is_clear[row] += is_clear[row+1];
        if (row_full) {
            if (row) is_clear[row-1] += 1;
            for (col=0; col<FIELD_COL; col++)
                cur_field[row][col] = 0;
        }
    }

    for (row=FIELD_ROW+FIELD_ROW_OFFSET-1; row>=top_row; row--) {
        if (!is_clear[row]) continue;
        for (col=0; col<FIELD_COL; col++) {
            if (row+is_clear[row]<FIELD_ROW+FIELD_ROW_OFFSET)
                cur_field[row+is_clear[row]][col] = cur_field[row][col];
            cur_field[row][col] = 0;
        }
    }
}

void set_ghost(TETRIMINO *cur_mino, TETRIMINO *ghost_mino) {
    int idx, y, x;
    ghost_mino->center->x = cur_mino->center->x;
    ghost_mino->center->y = cur_mino->center->y;
    ghost_mino->type = cur_mino->type;
    ghost_mino->rotation = cur_mino->rotation;
    move_tetrimino(ghost_mino, SPACE);
}

void shuffle(int is_back) {
    int idx, rn, tmp, offset, cell;
    offset = (is_back ? TETRIMINO_NUM : 0);
    for (idx=0; idx<TETRIMINO_NUM; idx++) {
        rn = rand()%TETRIMINO_NUM;
        tmp = nxt[idx+offset].type;
        nxt[idx+offset].type = nxt[rn+offset].type;
        nxt[rn+offset].type = tmp;
        colorize_tetrimino(&nxt[idx+offset]);
        colorize_tetrimino(&nxt[rn+offset]);
    }
}

void _show_nxt() {
    int idx, cell;
    for (idx=0; idx<NXT_SHOW; idx++) {
        for (cell=0; cell<4; cell++) {
            TETRIMINO *prv_mino = &nxt[(cur_nxt+idx+TETRIMINO_NUM*2-1)%(TETRIMINO_NUM*2)];
            int prv_Y = NXT_Y + tetrimino[prv_mino->type][0][cell].y + NXT_INTERVAL*idx;
            int prv_X = NXT_X + tetrimino[prv_mino->type][0][cell].x*2;
            gotoxy(prv_Y, prv_X);
            printw(" ");
        }
        for (cell=0; cell<4; cell++) {
            TETRIMINO *cur_mino = &nxt[(cur_nxt+idx)%(TETRIMINO_NUM*2)];
            int cur_Y = NXT_Y + tetrimino[cur_mino->type][0][cell].y + NXT_INTERVAL*idx;
            int cur_X = NXT_X + tetrimino[cur_mino->type][0][cell].x*2;
            gotoxy(cur_Y, cur_X);
            colorize(cur_mino->colors[cell]);
            printw("■");
            decolorize(cur_mino->colors[cell]);
        }
    }
}

void _clear_nxt() {
    int idx, cell;
    for (idx=0; idx<NXT_SHOW; idx++) {
        for (cell=0; cell<4; cell++) {
            TETRIMINO *cur_mino = &nxt[(cur_nxt+idx)%(TETRIMINO_NUM*2)];
            int cur_Y = NXT_Y + tetrimino[cur_mino->type][0][cell].y + NXT_INTERVAL*idx;
            int cur_X = NXT_X + tetrimino[cur_mino->type][0][cell].x*2;
            gotoxy(cur_Y, cur_X);
            printw(" ");
        }
    }
}

void increase_nxt(int *cur_nxt) {
    *cur_nxt = (*cur_nxt+1)%(TETRIMINO_NUM*2);
    if (*cur_nxt==2) shuffle(true);
    else if (*cur_nxt==TETRIMINO_NUM+2) shuffle(false);
}

void colorize(int type) {
    #ifdef __APPLE__
        switch (type-1) {
            case I_MINO: attron(COLOR_PAIR(RED_PAIR)); break;
            case J_MINO: attron(COLOR_PAIR(GREEN_PAIR)); break;
            case L_MINO: attron(COLOR_PAIR(YELLOW_PAIR)); break;
            case O_MINO: attron(COLOR_PAIR(BLUE_PAIR)); break;
            case S_MINO: attron(COLOR_PAIR(MAGENTA_PAIR)); break;
            case Z_MINO: attron(COLOR_PAIR(CYAN_PAIR)); break;
            case T_MINO: attron(COLOR_PAIR(WHITE_PAIR)); break;
            default: break;
        }
    #endif
    #ifdef _WIN32
        switch (type-1) {
            case I_MINO: printw("\e[0;31m"); break;
            case J_MINO: printw("\e[0;32m"); break;
            case L_MINO: printw("\e[0;33m"); break;
            case O_MINO: printw("\e[0;34m"); break;
            case S_MINO: printw("\e[0;35m"); break;
            case Z_MINO: printw("\e[0;36m"); break;
            case T_MINO: printw("\e[0;37m"); break;
            default: break;
        }
    #endif
}

void decolorize(int type) {
    #ifdef __APPLE__
        switch (type-1) {
            case I_MINO: attroff(COLOR_PAIR(RED_PAIR)); break;
            case J_MINO: attroff(COLOR_PAIR(GREEN_PAIR)); break;
            case L_MINO: attroff(COLOR_PAIR(YELLOW_PAIR)); break;
            case O_MINO: attroff(COLOR_PAIR(BLUE_PAIR)); break;
            case S_MINO: attroff(COLOR_PAIR(MAGENTA_PAIR)); break;
            case Z_MINO: attroff(COLOR_PAIR(CYAN_PAIR)); break;
            case T_MINO: attroff(COLOR_PAIR(WHITE_PAIR)); break;
            default: break;
        }
    #endif
    #ifdef _WIN32
        printw("\e[0;0m");
    #endif
}

void hold(TETRIMINO *hold_mino, TETRIMINO *cur_mino) {
    if (already_held) return;
    int prv_type, cell, prv_colors[4]={0,};

    prv_type = hold_mino->type;
    hold_mino->type = cur_mino->type;
    for (cell=0; cell<4; cell++) {
        prv_colors[cell] = hold_mino->colors[cell];
        hold_mino->colors[cell] = cur_mino->colors[cell];
    }
    CHNG(show_hold(prv_type, cur_mino));
    if (prv_type==-1) {
        cur_mino->type = nxt[cur_nxt].type;
        for (cell=0; cell<4; cell++)
            cur_mino->colors[cell] = nxt[cur_nxt].colors[cell];
        increase_nxt(&cur_nxt);
    } else {
        cur_mino->type = prv_type;
        for (cell=0; cell<4; cell++)
            cur_mino->colors[cell] = prv_colors[cell];
    }
    cur_mino->center->x = TETRIMINO_START_X;
    cur_mino->center->y = TETRIMINO_START_Y;
    cur_mino->rotation = 0;
    already_held = 1;
    return;
}

void _show_hold(int prv_type, TETRIMINO *hold_mino) {
    int cur_type = hold_mino->type;
    if (prv_type!=-1) {
        for (int cell=0; cell<4; cell++) {
            int prv_Y = HOLD_Y + tetrimino[prv_type][0][cell].y;
            int prv_X = HOLD_X + tetrimino[prv_type][0][cell].x*2+2;
            gotoxy(prv_Y, prv_X);
            printw(" ");
        }
    }

    if (cur_type!=-1) {
        for (int cell=0; cell<4; cell++) {
            int cur_Y = HOLD_Y + tetrimino[cur_type][0][cell].y;
            int cur_X = HOLD_X + tetrimino[cur_type][0][cell].x*2+2;
            gotoxy(cur_Y,cur_X);
            colorize(hold_mino->colors[cell]);
            printw("■");
            decolorize(hold_mino->colors[cell]);
        }
    }
}

void _clear_hold(int cur_type) {
    if (cur_type!=-1) {
        for (int cell=0; cell<4; cell++) {
            int cur_Y = HOLD_Y + tetrimino[cur_type][0][cell].y;
            int cur_X = HOLD_X + tetrimino[cur_type][0][cell].x*2+2;
            gotoxy(cur_Y,cur_X);
            printw(" ");
        }
    }
}

void unset_tetrimino(TETRIMINO *mino) {
    int idx;
    if (is_placed) {
        is_placed = false;
        return;
    }
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[mino->type][mino->rotation][idx];
        cur_field[mino->center->y+cell.y][mino->center->x+cell.x] = 0;
    }
}

void set_tetrimino(TETRIMINO *mino) {
    int idx;
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[mino->type][mino->rotation][idx];
        cur_field[mino->center->y+cell.y][mino->center->x+cell.x] = mino->colors[idx];
    }
}

int move_tetrimino(TETRIMINO *mino, int dir) {
    if (!is_valid_move(mino, dir)) return 0;

    if (dir==LEFT) mino->center->x--;
    else if (dir==RIGHT) mino->center->x++;
    else if (dir==DOWN) mino->center->y++;
    else if (dir==SPACE) {
        while (is_valid_move(mino, DOWN))
            mino->center->y++;
    }
    return 1;
}

int rotate_tetrimino(TETRIMINO *mino, int dir) {
    if (!is_valid_move(mino, dir)) return 0;

    if (dir==UP) mino->rotation = (mino->rotation+1)%ROTATION_NUM;
    else if (dir==REV_UP) mino->rotation = (mino->rotation+ROTATION_NUM-1)%ROTATION_NUM;
    else if (dir==UPSIDE_DOWN) mino->rotation = (mino->rotation+2)%ROTATION_NUM;
    return 1;
}

int is_valid_move(TETRIMINO *cur_mino, int dir) {
    int idx, cur_mino_idx, is_cur_mino;
    COORDINATE center = {cur_mino->center->x, cur_mino->center->y};
    TETRIMINO mino = *cur_mino;

    mino.center = &center;
    if (dir==LEFT) mino.center->x--;
    else if (dir==RIGHT) mino.center->x++;
    else if (dir==DOWN) mino.center->y++;
    else if (dir==UP) mino.rotation = (mino.rotation+1)%ROTATION_NUM;
    else if (dir==REV_UP) mino.rotation = (mino.rotation+ROTATION_NUM-1)%ROTATION_NUM;
    else if (dir==UPSIDE_DOWN) mino.rotation = (mino.rotation+2)%ROTATION_NUM;
    
    for (idx=0; idx<4; idx++) {
        COORDINATE cell = tetrimino[mino.type][mino.rotation][idx];
        int cell_Y = mino.center->y + cell.y;
        int cell_X = mino.center->x + cell.x;
        if (
            0>cell_Y || cell_Y>=FIELD_ROW+FIELD_ROW_OFFSET ||
            0>cell_X || cell_X>=FIELD_COL
        ) return 0;
        is_cur_mino = 0;
        for (cur_mino_idx=0; cur_mino_idx<4; cur_mino_idx++) {
            COORDINATE cur_cell = tetrimino[cur_mino->type][cur_mino->rotation][cur_mino_idx];
            int cur_cell_Y = cur_mino->center->y + cur_cell.y;
            int cur_cell_X = cur_mino->center->x + cur_cell.x;
            if (cur_cell_X==cell_X && cur_cell_Y==cell_Y) {is_cur_mino = 1; break;}
        }
        if (is_cur_mino) continue;
        if (cur_field[cell_Y][cell_X]>0) return 0;
    }
    return 1;
}

void crashed(TETRIMINO *prv_mino, TETRIMINO *cur_mino, TETRIMINO *ghost_mino) {
    is_placed = 1;
    milisec = 0;
    check_clear();
    cur_mino->type = nxt[cur_nxt].type;
    for (int cell=0; cell<4; cell++)
        cur_mino->colors[cell] = nxt[cur_nxt].colors[cell];
    cur_mino->rotation = 0;
    cur_mino->center->x = TETRIMINO_START_X;
    cur_mino->center->y = TETRIMINO_START_Y;
    increase_nxt(&cur_nxt);
    already_held = 0;
}

void colorize_tetrimino(TETRIMINO *cur_mino) {
    int cell, type;
    type = cur_mino->type;
    if (0>type || type>TETRIMINO_NUM) return;
    for (cell=0; cell<4; cell++)
        cur_mino->colors[cell] = type+1;
}
