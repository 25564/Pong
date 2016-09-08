#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define DELAY 10 /* milliseconds */
#define PADDLE_LENGTH 6

// Structs
typedef struct {
    bool gameOver;
    bool isPaused;
    int level;
    int score;
    int lives;
} GameState;

typedef struct {
    int h; // Hour - Probably overkill but I would be pissed if it was what broke it
    int m; // Minute
    int s; // Second
    int ms;// MilliSecond
} GameTimer;

typedef struct {
    int height;
    int x;
    int y;
} Paddle;

GameState gameState = {false,false,1,0,3};
GameTimer Timer = {0,0,0,0};

Paddle RightPaddle = {7, 1, 1};
Paddle LeftPaddle = {7, 1, 1};

// SCREEN BOUNDARIES
int ScreenWidth;
int ScreenHeight;
int TopPlayWall;
int BottomPlayWall;
int LeftPlayAreaWall;
int RightPlayAreaWall;

// Images
char BorderChar = '*';
char BallChar[] = "O";

// Sprites
sprite_id TickerSprite;
sprite_id ball;

// Prototypes
void DrawBorderBox(void);
void process(void);
void setup(void);
int PaddleHeight(void);
void SetupPaddles(void);
void MakePaddles(void);
void displayCountDown(void);
void checkInputs(void);
void movePaddle(int);
void BallBounce(double, double);
void checkBallCollision(void);
void EstablishConstants(void);

void EstablishConstants(void) {
    ScreenWidth = screen_width()-1;
    ScreenHeight = screen_height()-1;

    TopPlayWall = 3;
    BottomPlayWall = ScreenHeight-1;
    LeftPlayAreaWall = 1;
    RightPlayAreaWall = ScreenWidth-1;
}

void setup(void) {
    displayCountDown();

    SetupPaddles();

    ball = sprite_create(ScreenWidth/2, ScreenHeight/2, 1, 1, BallChar);
    sprite_turn_to(ball, -0.15, -0.15);
}

// Draw screen borders and HUD
void DrawBorderBox(void){

    // Page Borders
    draw_line(0, 0, ScreenWidth, 0, BorderChar);
    draw_line(0, ScreenHeight, ScreenWidth, ScreenHeight, BorderChar);
    draw_line(0, 0, 0, ScreenHeight, BorderChar);
    draw_line(ScreenWidth, 0, ScreenWidth, ScreenHeight, BorderChar);

    int gutter = (ScreenWidth+2)/4;
    draw_line(0, 2, ScreenWidth, 2, BorderChar);

    // Hud Stats
    draw_formatted(2, 1, "Lives = %d", gameState.lives);
    draw_formatted(2+(gutter), 1, "Score = %d", gameState.score);
    draw_formatted(2+(gutter*2), 1, "Level = %d", gameState.level);

    if (Timer.s >= 10){
        draw_formatted(2+(gutter*3), 1, "Time = %d:%d", Timer.m, Timer.s);
    } else {
        draw_formatted(2+(gutter*3), 1, "Time = %d:0%d", Timer.m, Timer.s);
    }
}

void BallBounce(double x, double y){
    sprite_back(ball);
    sprite_turn_to(ball, sprite_dx(ball)*x, sprite_dy(ball)*y);
}

int PaddleHeight(void) {
    if (ScreenHeight > 21){
        return 7;
    }
    return((ScreenHeight-3-1)/2);
}

void SetupPaddles() {
    LeftPaddle.height = PaddleHeight();
    RightPaddle.height = PaddleHeight();

    LeftPaddle.x = LeftPlayAreaWall + 2;
    RightPaddle.x = RightPlayAreaWall - 2;

    LeftPaddle.y = (ScreenHeight/2)-(PaddleHeight()/2);
    RightPaddle.y = (ScreenHeight/2)-(PaddleHeight()/2);
}

void MakePaddles (void) {
    if (gameState.level > 1) {
        draw_line(LeftPaddle.x, LeftPaddle.y, LeftPaddle.x, LeftPaddle.y+LeftPaddle.height, '|');
    }
    draw_line(RightPaddle.x, RightPaddle.y, RightPaddle.x, RightPaddle.y+RightPaddle.height, '|');
}

void displayCountDown(void) {
    clear_screen();
    for (int i = 3; i > 0; i--){
        char Template[] = 
        /**/          "-------"
        /**/          "|     |"
        /**/          "|  %d  |"
        /**/          "|     |" 
        /**/          "-------";

        char Output[99];
        sprintf(Output, Template, i); // Screw Subbing Ints into Strings
        TickerSprite = sprite_create((ScreenWidth/2)-3, (ScreenHeight/2)-3, 7, 5, Output);    
        sprite_draw(TickerSprite);

        show_screen();
        timer_pause(1000);
    }

    clear_screen();
    draw_string(ScreenWidth/2-7, ScreenHeight/2, "Try not to Die");
    show_screen();
    timer_pause(1000);
}

void movePaddle(int direction) {
    int newY = RightPaddle.y + direction;
    if(newY >= TopPlayWall && (newY + RightPaddle.height) <= BottomPlayWall) {
        RightPaddle.y = RightPaddle.y + direction;
    }
}

void checkInputs(void) {
    switch (get_char()) {
        case 'w':
            movePaddle(-1);
            break;
        case 's':
            movePaddle(1);
            break;
        case 'l':
            gameState.level = (gameState.level == 4) ? 1 : gameState.level + 1;
        default:
            break;
    }
}

void DisplayHelpScreen(void){
    clear_screen();
    draw_string(ScreenWidth/2-21, ScreenHeight/2-5, "Cian O'Leary; n9727442; CAB202; 2016 Sem 2");
    draw_string(ScreenWidth/2-11, ScreenHeight/2-3, "~~ Crappy Pong Game ~~");
    draw_string(ScreenWidth/2-10, ScreenHeight/2-1, "'l' to change levels");
    draw_string(ScreenWidth/2-6, ScreenHeight/2, "'r' to reset");
    draw_string(ScreenWidth/2-12, ScreenHeight/2-1, "'h' to display help text");
    draw_string(ScreenWidth/2-7, ScreenHeight/2+4, "--- Rules ---");
    draw_string(ScreenWidth/2-10, ScreenHeight/2+6, "'w' to move paddle up");
    draw_string(ScreenWidth/2-11, ScreenHeight/2+7, "'s' to move paddle down");

    show_screen();
    wait_char();
    clear_screen();
}

void checkBallCollision(void) {
    int ballX = sprite_x(ball);
    int ballY = sprite_y(ball);

    // Wall Collisions
    if (ballY < TopPlayWall) { // Top
        BallBounce(1, -1);
    } else if (ballY >= BottomPlayWall) { // Bottom
        BallBounce(1, -1);
    }

    if (gameState.level == 1) {// Level One Left Wall
        if (ballX < LeftPlayAreaWall) {
            BallBounce(-1, 1);
        }
    } else {
        // It is impossible for the robot player to miss so why not
        if (ballX <= LeftPlayAreaWall + 2) {
            BallBounce(-1, 1);
        }
    }

    if (ballX >= RightPlayAreaWall) { // Player screwed up
        gameState.lives -= 1;
        if (gameState.lives <= 0){
            gameState.gameOver = true;
        } else{
            setup();
        }
    }

    // Paddle Logic
    if (ballX >= RightPaddle.x - 1 && ballX <= RightPaddle.x){
        // Top of the paddle
        if (ballY == RightPaddle.y){
            if (sprite_dy(ball) > 0){
                if (RightPaddle.y - TopPlayWall > 1){
                    BallBounce(1.0, -1.0);
                } else {
                    BallBounce(-1.0, -1.0);
                }
            } else {
                BallBounce(-1.0, 1.0);
            }
        } else if (ballY>= RightPaddle.y+1 && ballY <= RightPaddle.y + RightPaddle.height - 1){
            BallBounce(-1.0, 1.0);
        } else if (ballY > RightPaddle.y + RightPaddle.height - 1 && ballY <= RightPaddle.y + RightPaddle.height){
            // Bottom up not middle out.
            if (sprite_dy(ball) < 0){
                if (BottomPlayWall - (RightPaddle.y+RightPaddle.height) > 1){
                    BallBounce(-1.0, -1.0);
                }
            } else {
                BallBounce(-1.0, 1.0);
            }
        }
        gameState.score += 1;
    }
}

// One more time around jeeves
void process(void) {
    clear_screen();

    //  We will build a wall and make the paddle pay for it.
    DrawBorderBox();
    checkInputs();
    MakePaddles();

    sprite_step(ball);
    checkBallCollision();

    // Bot Paddle
    if (gameState.level > 1) {
        LeftPaddle.y = sprite_y(ball) - (LeftPaddle.height/2);
        if (LeftPaddle.y < TopPlayWall) {
            LeftPaddle.y = TopPlayWall;
        } else if (LeftPaddle.y + LeftPaddle.height > BottomPlayWall) {
            LeftPaddle.y = BottomPlayWall - LeftPaddle.height;
        }
    }

    sprite_draw(ball);
}

void cleanup(void) {
}

// Program entry point.
int main(void) {
    setup_screen();

#if defined(AUTO_SAVE_SCREEN)
    auto_save_screen(true);
#endif

    EstablishConstants();

    do{
        DisplayHelpScreen();
        setup();
        while ( gameState.gameOver == false ) {
            process();
            show_screen();
            timer_pause(DELAY);

            // You have no clue how much distain I have for timers now

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
        draw_string(ScreenWidth/2-12, ScreenHeight/2, "Rip, Want to play again? (y/n)");
        draw_string(ScreenWidth/2-12, ScreenHeight/2, "Rip, Want to play again? (y/n)");
        show_screen();
    }while(wait_char() == 'y');

    cleanup();

    return 0;
}
