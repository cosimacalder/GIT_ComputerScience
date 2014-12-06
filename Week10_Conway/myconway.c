/*  CONWAY'S SOlDIERS

  - User enters coordinates of square via argv[1] and argv[2] followed by SDL or BASIC
  - Soldier can move two spaces in any of the four orthogonal directions...
  - Provided the space it jumps over contains a peg and the space it ends up in is empty.
  - Linked list of boards, parent and next

  - hashing assigns each board a unique identity instead of sequential search ***************
  - use a short instead of int ?
  - PAGODA FUNCTION

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neillsdl2.h"

#define COL 7
#define ROW 8
#define HALF 4
#define TRUE 1
#define FALSE 0
#define MILLISECONDDELAY 300
#define PEG 'x'
#define SPACE '.'

typedef struct node
{
    char board[ROW][COL]; //Board
    struct node *parent; //Board's parent
    struct node *next;
} Node;

int check_input(int argc, char *argv[], int x, int y);
void check_arg(int argc, char *argv[], int x, int y);
void display_SDL(Node *currentBoard);
void print_board(Node *newMoves);
//void print_reverse(Node *newMoves);

Node *initialise_board(Node *initBoard);
Node *compute_moves(Node *currentBoard);
Node *copy_board(Node *currentBoard);
int check_board(Node *compareA, Node *compareB);
int checkSol(int x, int y, Node *currentBoard);


int main(int argc, char *argv[])
{
    int y = atoi(argv[1]);
    int x = atoi(argv[2]);

    int SDL = 0; // SDL flag

    Node *currentBoard = NULL;
    Node *newMoves = NULL;
    Node *newMovestmp = NULL;
    Node *currentBoardtmp = NULL;
    Node *endBoard = NULL;
    Node *startBoard = NULL;


    SDL = check_input(argc, argv, x, y);
    currentBoard = initialise_board(currentBoard);

    startBoard = currentBoard; //saves pointer to very start of linked list

    /*remove duplicates*/
    newMovestmp = newMoves;
    currentBoardtmp = startBoard;
    


    /*iterate through whole of linked list*/
    while (currentBoard != NULL)
    {
        if (checkSol(x, y, currentBoard))
        {
            printf("Found solution!\n");
            while (currentBoard != NULL)
            {
                if (SDL == TRUE)
                {
                    display_SDL(currentBoard);
                    currentBoard = currentBoard->parent;
                }
                else
                {
                    print_board(currentBoard); //currently prints backwards ???
                    //print_reverse(currentBoard);
                    currentBoard = currentBoard->parent;
                    getchar();
                }
            }
            exit(0);
        }
        /*compute possible new moves for current board*/
        newMoves = compute_moves(currentBoard);

        endBoard = currentBoard; //end of current board
        while (endBoard->next != NULL)
        {
            endBoard = endBoard->next;
        }
        endBoard->next = newMoves; //add new moves on to end of list
        currentBoard = currentBoard->next; //shift current board down list

        // /*witerate through current boards and new moves checking for duplicates*/
        // while(newMovestmp != NULL)
        // {
        //   while (currentBoardtmp != NULL)
        //   {
        //     if(!check_board(newMovestmp, currentBoardtmp)) //if not a duplicate
        //     {
        //       //printf("unique\n");
        //       endBoard->next = newMovestmp;
        //       endBoard = newMovestmp;
        //     }
        //     //print_board(currentBoardtmp);
        //     currentBoardtmp = currentBoardtmp->next; //else move on to next board in list
        //   }
        //   printf("aa\n");

        //   newMovestmp = newMovestmp->next; //when moving through newMoves
        //   currentBoardtmp = startBoard; //reset currentboard temp
        // }
        // endBoard->next = NULL;
        // currentBoardtmp = currentBoard;
    }

    return 0;
}

Node *initialise_board(Node *initBoard)
{
    int row, col;
    initBoard = (Node *)malloc(sizeof(Node));
    initBoard->next = NULL;
    initBoard->parent = NULL;

    if (initBoard == NULL)
    {
        printf("\nThe malloc failed\n");
        exit(0);
    }

    for (row = 0; row < HALF; row++) //initialise first half with x's
    {
        for (col = 0; col < COL; col++)
        {
            initBoard->board[row][col] = PEG;
        }
    }
    for (row = HALF; row < ROW; row++) //and second half with .'s'
    {
        for (col = 0; col < COL; col++)
        {
            initBoard->board[row][col] = SPACE;
        }
    }
    return initBoard;
}

int check_input(int argc, char *argv[], int x, int y)
{
    check_arg(argc, argv, x, y);
    int SDL = 0; // SDL flag

    if (strcmp(argv[3], "SDL") == 0)
    {
        printf("\nSDL initialised!\n\n");
        SDL = TRUE;
    }
    else if (strcmp(argv[3], "BASIC") == 0)
    {
        printf("\nSDL option not chosen, view basic in terminal:\n\n");
        SDL = FALSE;
    }
    else
    {
        printf("\nERROR: Please choose SDL or BASIC.\n\n");
        exit(0);
    }
    return SDL;
}

void check_arg(int argc, char *argv[], int x, int y)
{
    if (argc > 3)
    {
        if (y > COL || y < 0 || x < (HALF) || x > ROW)
        {
            printf( "\nERROR: Your target location is not within the unoccupied board.\nThe BOTTOM half of the board is empty.\n\n");
            exit(0);
        }
    }
    else
    {
        printf( "\nERROR: Please enter 2 coordinates AND a display option.\n\n" ); //THIS DOESNT WORK FOR < 3 ???
        exit(0);
    }
}

void display_SDL(Node *currentBoard)
{
    SDL_Simplewin sw;
    int cellsize = WHEIGHT / 7;
    int offset_x = cellsize / 4;
    int offset_y = cellsize / 2;
    int row = 0, col = 0;

    Neill_SDL_Init(&sw);

    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            if (currentBoard->board[row][col] == PEG)
            {
                Neill_SDL_SetDrawColour(&sw, 0, 175, 150);
                Neill_SDL_RenderFillCircle(sw.renderer, offset_x + (cellsize) * (row + 1), (cellsize) * (col + 1) - offset_y, cellsize / 2.2);
            }
            if (currentBoard->board[row][col] == SPACE)
            {
                Neill_SDL_SetDrawColour(&sw, 255, 255, 255);
                Neill_SDL_RenderFillCircle(sw.renderer, offset_x + (cellsize) * (row + 1), (cellsize) * (col + 1) - offset_y, cellsize / 10);
            }
        }
    }
    SDL_RenderPresent(sw.renderer);
    SDL_UpdateWindowSurface(sw.win);
    SDL_Delay(MILLISECONDDELAY);  // Wait a short time

    while (!sw.finished)
    {
        Neill_SDL_Events(&sw);
    }
}

Node *compute_moves(Node *currentBoard)
{
    int col, row;
    Node *listhead = NULL;

    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            if (currentBoard->board[row][col] == PEG)
            {
                //printf("i am here\n");
                if (row < (ROW - 2) && currentBoard->board[row + 1][col] == PEG
                        && currentBoard->board[row + 2][col] == SPACE)
                {
                    Node *newboard = copy_board(currentBoard);
                    newboard->parent = currentBoard;
                    newboard->next = listhead;
                    newboard->board[row][col] = SPACE;
                    newboard->board[row + 1][col] = SPACE;
                    newboard->board[row + 2][col] = PEG;
                    listhead = newboard;
                }
                if (row > 1 && currentBoard->board[row - 1][col] == PEG
                        && currentBoard->board[row - 2][col] == SPACE)
                {
                    Node *newboard = copy_board(currentBoard);
                    newboard->parent = currentBoard;
                    newboard->next = listhead;
                    newboard->board[row][col] = SPACE;
                    newboard->board[row - 1][col] = SPACE;
                    newboard->board[row - 1][col] = PEG;
                    listhead = newboard;
                }
                if (col < (COL - 2) && currentBoard->board[row][col + 1] == PEG
                        && currentBoard->board[row][col + 2] == SPACE)
                {
                    Node *newboard = copy_board(currentBoard);
                    newboard->parent = currentBoard;
                    newboard->next = listhead;
                    newboard->board[row][col] = SPACE;
                    newboard->board[row][col + 1] = SPACE;
                    newboard->board[row][col + 2] = PEG;
                    listhead = newboard;
                }
                if (col > 1 && currentBoard->board[row][col - 1] == PEG
                        && currentBoard->board[row][col - 2] == SPACE)
                {
                    Node *newboard = copy_board(currentBoard);
                    newboard->parent = currentBoard;
                    newboard->next = listhead;
                    newboard->board[row][col] = SPACE;
                    newboard->board[row][col - 1] = SPACE;
                    newboard->board[row][col - 2] = PEG;
                    listhead = newboard;
                }
            }
        }
    }
    return listhead;
}

Node *copy_board(Node *currentBoard)
{
    int row, col;
    Node *newNode;
    newNode = (Node *)malloc(sizeof(Node));

    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            newNode->board[row][col] = currentBoard->board[row][col];
        }
    }
    return newNode;
}

void print_board(Node *newMoves)
{
    int row, col;
    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            printf("%c", newMoves->board[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}


int check_board(Node *compareA, Node *compareB)
{
    int row, col;

    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            if (strcmp(&compareA->board[row][col], &compareB->board[row][col]) != 0)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

int checkSol(int x, int y, Node *currentBoard)
{
    if (currentBoard->board[y][x] == 'x')
    {
        return TRUE;
    }
    return FALSE;
}

// void print_reverse(Node *newMoves)
// {
//     int row, col;
//     // Base case
//     if (newMoves == NULL)
//         return;
//     for (row = 0; row < ROW; row++)
//     {
//         for (col = 0; col < COL; col++)
//         {
//             // print the list after head node
//             print_reverse(newMoves->next);

//             // After everything else is printed, print head
//             printf("%c", newMoves->board[row][col]);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

