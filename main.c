#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define DELAY 10 /* milliseconds */
#define PADDLE_LENGTH 6

// Structs
typedef struct GameStates{
    bool gameOver;
    bool isPaused;
    int level;
    int score;
    int lives;
} GameState;

typedef struct Timer {
    int h; // Probably overkill but I would be pissed if it was what broke it
    int m;
    int s;
    int ms;
} GameTimer;

GameState gameState = {false,false,0,0,3};
GameTimer Timer = {0,0,0,0};

// SCREEN BOUNDARIES
int SCREEN_WIDTH;
int SCREEN_HEIGHT;
int TOP_PLAY_AREA_WALL;
int BOTTOM_PLAY_AREA_WALL;
int LEFT_PLAY_AREA_WALL;
int RIGHT_PLAY_AREA_WALL;

// Images
char BorderChar = '*';

// Prototypes
void DrawBorderBox(void);
void process(void);
void countdown(void);

// Setup our variables and sprites
void setup(void) {
    SCREEN_WIDTH = screen_width()-1;
    SCREEN_HEIGHT = screen_height()-1;

    TOP_PLAY_AREA_WALL = 3;
    BOTTOM_PLAY_AREA_WALL = SCREEN_HEIGHT-1;
    LEFT_PLAY_AREA_WALL = 1;
    RIGHT_PLAY_AREA_WALL = SCREEN_WIDTH-1; 
}

// Draw screen borders and HUD
void DrawBorderBox(void){

    // Page Borders
    draw_line(0, 0, SCREEN_WIDTH, 0, BorderChar);
    draw_line(0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, BorderChar);
    draw_line(0, 0, 0, SCREEN_HEIGHT, BorderChar);
    draw_line(SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BorderChar);

    int gutter = (SCREEN_WIDTH+2)/4;
    draw_line(0, 2, SCREEN_WIDTH, 2, BorderChar);


    //Note to self: Find a better way to print int in strings
    //draw_string(2, 1, "Lives:")
    //draw_int(8, 1, 1)

    // This is hopefully large enough?
    char Output[99];

    sprintf(Output, "Lives = %d", gameState.lives);
    draw_string(2, 1, Output);

    sprintf(Output, "Score = %d", gameState.score);
    draw_string(2+(gutter), 1, Output);

    sprintf(Output, "Level = %d", gameState.level);
    draw_string(2+(gutter*2), 1, Output);

    if (Timer.s >= 10){
        sprintf(Output, "Time = %d:%d", Timer.m, Timer.s);
    } else {
        sprintf(Output, "Time = %d:0%d", Timer.m, Timer.s);
    }

    draw_string(2+(gutter*3), 1, Output);
}


// One more time aroud jeeves
void process(void) {
    clear_screen();

    //  We will build a wall and make the paddle pay for it.
    DrawBorderBox();
}

// Clean up game
void cleanup(void) {
    // No likey memory leaky
    //free(rails);
}

// Program entry point.
int main(void) {
    setup_screen();

#if defined(AUTO_SAVE_SCREEN)
    auto_save_screen(true);
#endif

    do{
        setup();
        while ( gameState.gameOver == false ) {
            process();
            show_screen();
            timer_pause(DELAY);

            Timer.ms += DELAY;

            if (Timer.ms >= 1000){
                Timer.s += 1;
                Timer.ms = 0;
            }
            if (Timer.s >= 60){
                Timer.m += 1;
                Timer.s = 0;
            }
            if (Timer.m >= 60){
                Timer.h += 1;
                Timer.m = 0;
            }
        }        

        clear_screen();

        draw_string(SCREEN_WIDTH/2-14, SCREEN_HEIGHT/2, "Game over, play again? (y/n)");
        show_screen();
    }while(wait_char() == 'y');

    cleanup();

    return 0;
}
