/*      ___   ___  ___  ____  ____
 *     / _ \ / __|/  _||_  _||_  _|
 *    | |_| |\__ \| |_  _||_  _||_
 *    |_| |_||___/\___||____||____|                   
 *        ___           _         
 *       / __|_ _  __ _| |_____ 
 *       \__ \ ' \/ _` | / / -_)  
 *       |___/_||_\__,_|_\_\___
*/

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Define border ascii
// left side
#define ls '>'
// right side
#define rs '<'
// top side
#define ts '='
// bottom side
#define bs '='
// corner
#define crnr '*'

// Define snake

// Define snake starting length
#define length 20

// Define snake ascii
#define HEAD '@'
#define BODY 'O'
#define FRUIT '$'


// Define snake node structure 
// (snake works as a linked list, the head and each body char represents a node)
struct snakenode {
    int y;
    int x;
    struct snakenode* next;
};
// Define direction values
#define up 1
#define down -1
#define right 2
#define left -2

// Define game info strings
#define exitstring " | Press ""x"" to end game | "
#define scorestring "HI-SCORE: %d | SCORE: %d | "

// Declare score variables
int highscore = 0;
int score, scorestart;

// Declare game over message strings

// Declare game variables
int gameover, direction, width, height;
int fruitx, fruity, prevtailx, prevtaily, flag, prevflag, fruitflag;
struct snakenode *head, *tail, *cur;

// Declare ncurses window variables
WINDOW *game, *info;

// Declare function prototypes
void init();
void title();
void initgame();
void setup();
struct snakenode* addnode(int y, int x);
void initsnake();
void fruitgen();
void drawscore();
void updatescore();
void input();
void logic();
void updatesnake();
void shiftsnake();
void draw();
int retry();
void freesnake();
int kbhit(void);
int halfround(int len);
int halfstrlen (char* string);

// Driver Code
int main(void)
{
    // initialize ncurses
        init();

    do
    {
        // Initialize game related ncurses
        initgame();
        
        // Setup game variables and environment
	    setup();
	
        // Until the game is over
	    while (!gameover) {

	        // Function Call
            input();
            updatesnake();
		    logic();
	        draw();
            usleep(90000);
	    
        }
        // Free snake nodes
        freesnake();
    }
    while (retry());
    
    // Close ncurses window
    endwin();


    return 0;
}

// initialize ncurses
void init()
{
    // main ncurses config
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    
    // If terminal is large enough, display title screen
    if (COLS > 28 && LINES > 10)
        title();
}

// initial game setup
void initgame()
{
// initialize game windows
    nodelay(stdscr, true);
    game = newwin(LINES - 1, COLS, 0, 0);
    info = newwin(1, COLS, LINES - 1, 0);
    
    // 
    getmaxyx(game, height, width);
    width--;
    height--;

    clear();
    refresh();

}

// Display title screen
void title()
{
    char* art[] = {
     "  ___   ___  ___  ____  ____ ",
     " / _ \\ / __|/  _||_  _||_  _|",
     "| |_| |\\__ \\| |_  _||_  _||_ ",
     "|_| |_||___/\\___||____||____|",                   
     "    ___           _          ",
     "   / __|_ _  __ _| |_____    ",
     "   \\__ \\ ' \\/ _` | / / -_)   ",
     "   |___/_||_\\__,_|_\\_\\___    ",
     "                             ",
     "   press any key to start    "
    };
    
    // print art to screen
    for (int i = 0; i < 10; i++)
        mvprintw(LINES / 2 - 5 + i, COLS / 2 - 14, art[i]);
    refresh();
    
    // Look for input to start game
    int ch = getch();
    clear();
    refresh();
}

void setup()
{
	// Draw game border 
    wborder(game,ls, rs, ts, bs, crnr, crnr, crnr, crnr);
    wrefresh(game);
    
    // Initialize Info window
    // Find half the length of exit and score string combined
    int halfinfolen = halfround(strlen(exitstring) + strlen(scorestring));
    // move cursor to middle of info window, minux halfinfolen
    wmove(info, 0, COLS/2 - halfinfolen);
    
    // Print exitstring 
    wprintw(info, exitstring);
    
    // Store cursor start position of score info
    int y;
    getyx(info, y, scorestart); 
   
    // Set score to 0
    score = 0;

    // Draw score
    drawscore();
    
    
    // Initialize game variables
    gameover = 0;

    fruitflag = 0;

    // Initialize fruit and snake
    fruitgen();
    
    initsnake();
    
}

// Add node
struct snakenode* addnode(int y, int x)
{
    struct snakenode* newnode;
    newnode = malloc(sizeof(struct snakenode));
    newnode->x = x;
    newnode->y = y;
    newnode->next = NULL;

    return newnode;
}

// initialize snake
void initsnake()
{
    direction = 2;
   
    box(game, 0, 0);
    
    flag = 2;

    tail = addnode(height/2, width/2 - length);

    head = tail;
    
    for (int i = 1; i <= length; i++)
    {
        head->next = addnode(tail->y, tail->x + i);
        head = head->next;
    }

    prevtailx = tail->x - 1;
    prevtaily = tail->y;
}

// Generate fruit location
void fruitgen()
{
    srand(time(NULL));

    do
	    fruitx = rand() % width;
	while (fruitx == 0);
    
    do
	    fruity = rand() % height;
	while (fruity == 0);
}
// Draw score
void drawscore()
{

    // Draw score
    mvwprintw(info, 0, scorestart, scorestring, highscore, score);
    
    // Refresh window
    wrefresh(info);

}

// Update score
void updatescore()
{   
    // Increment score
    score++;

    // Update high score if warranted 
    if (score > highscore)
        highscore = score;
    
    drawscore();
}

// Function to take the input
void input()
{
	if (kbhit()) {
		switch (getch()) {
        case KEY_LEFT:
        case 'a':
			flag = left;
			break;
        case KEY_DOWN:
        case 's':
			flag = down;
			break;
        case KEY_RIGHT:
        case 'd':
			flag = right;
			break;
        case KEY_UP:
        case 'w':
			flag = up;
			break;
        case 'x':
			gameover = 1;
			break;
		}
        
        // Flush ncurses input buffer to avoid possible input delay    
            flushinp();
	}
    
   }

// Function for the logic behind
// each movement
void logic()
{
    // check if direction is valid
    if (direction + flag != 0)
        direction = flag;

	// check snake collision with wall
	if (head->x <= 0 || head->x >= width
		|| head->y <= 0 || head->y >= height)
		gameover = 1;

    //check snake head collison with body
    if (mvinch(head->y, head->x) == BODY)
        gameover = 1;

	// Check fruit collision
	if (head->x == fruitx && head->y == fruity) {

        fruitgen();
        fruitflag = 1;
        updatescore();
	}
}

// calls to shift snake body then moves head based on direction
void updatesnake()
{
    // Shift snake node positions down snake
    shiftsnake();

    // Update head position
    switch (direction) {
		case up:
			head->y -= 1;
			break;
		case down:
			head->y += 1;
			break;
		case right:
			head->x += 1;
			break;
		case left:
			head->x -= 1;
			break;
    }
}

// Shift snake node positions down snake
void shiftsnake()
{
    // Put cursor to current tail
    cur = tail;
    
    // If fruit eaten
    if (fruitflag)
    {
        tail = addnode(tail->y, tail->x);
        tail->next = cur;
        fruitflag = 0;
    }
    else
    {
        // Store previous tail node
        prevtaily = tail->y;
        prevtailx = tail->x;
    }

        
    do
    {
        // move vector to next node
        cur->x = cur->next->x;
        cur->y = cur->next->y;
        
        // move tmp to next node
        cur = cur->next;
    }
    while (cur->next != NULL);
}

// Draw game
void draw()
{ 

    // Erase where previous tail
    mvaddch(prevtaily, prevtailx, ' ');
    
    // snake body
    struct snakenode *tmp = tail;

    while (tmp->next != NULL)
    {
        mvaddch(tmp->y, tmp->x, BODY);
        tmp = tmp->next;
    }
    
    // snake head
    mvaddch(head->y, head->x, HEAD);

    // fruit
    mvaddch(fruity, fruitx, FRUIT);

    refresh();
}

// Prompt player to retry or quit upon gameover
int retry()
{
    // Define gameover/ retry message
    char* messages [] = 
    {
        "*GAMEOVER*",
        "Try Again?",
        "y/n"
    };
    
    // Declare height offset
    int hoffset = -2;
    
    // Draw message
    for (int i = 0; i < 3; i++)
    {
        move(height/2 + hoffset, width/2 - halfstrlen(messages[i]));
        printw(messages[i]);
        hoffset += 2;
    }

    refresh();
    
    // Turn on input delay (waits for player to hit y or n before further code execution)
    nodelay(stdscr, false);
    
    int ch;

    do
    {
        ch = getch();

        if (ch == 'y')
            return 1;
        else if (ch == 'n')
            return 0;
    }
    while (ch);

}

// Free nodes
void freesnake()
{
    struct snakenode* tmp;

    while (tail != NULL)
    {
        tmp = tail;
        tail = tail->next;
        free(tmp);
    }
}

// Function to detect if a key is hit or not
int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

// Half the input value and round it up if input is odd
int halfround(int len)
{
// if length is uneven, round up
    if (len % 2 != 0)
        return len / 2 + 1;
    else
        return len / 2;
}

// Returns half string length of inputted string
int halfstrlen (char* string)
{
    // Find string length
    int len = strlen(string);
    
    // Return half of string length
    return halfround(len);
    }


