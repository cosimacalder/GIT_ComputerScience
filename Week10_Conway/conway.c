#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BCOL (8)
#define BROW (9)

typedef struct node{
    char board[BCOL][BROW];   //Board
    int boardnum;
    struct node *parent;      //Board's parent
    struct node *next;        //Node that follows
} Node;

/*Queue Structure*/
typedef struct qu{
    Node *headQ;      //First node in the queue
    Node *tailQ;      //Last node in the queue
} Qu;

void printboard(Node *activeB);
void addboard(Node *tmpN, Qu *queue);
void printsol(Node *lastBoard);
void playboard(Node *boardN, int x, int y);

int main(int argc, char *argv[]){
    Node *boardN;
    int x, y, col, row;
    boardN = (Node *)malloc(sizeof(Node));
    
    if (boardN == NULL){
        printf("\nThe malloc failed\n");
        exit(0);
    }
    /*Retrieve target location*/
    if (argc > 2){
        x = atoi(argv[1]);
        y = atoi(argv[2]);
        if (x > 6 || y > 3 || x < 0 || y < 0){
            printf( "Your target location is not within the confines of the unoccupied board\n" );
            exit(0);
        }
    }
    else{
        printf( "You have entered an incorrect target location\n" );
        exit(0);
    }
    
    /*Intializing board*/
    for (row = 0; row < 4; row++){
        for (col = 0; col < 7; col++){
            boardN->board[col][row] = '.';
        }
    }
    for (row = 4; row < 8; row++){
        for (col = 0; col < 7; col++){
            boardN->board[col][row] = 'O';
        }
    }

    boardN->parent = NULL;
    boardN->next = NULL;
    playboard(boardN, x, y);
    return 0;
}

void printboard(Node *activeB){
    int row = 0, col = 0;
    for (row = 0; row < 8; row++){
        for (col = 0; col < 7; col++){
            printf("%c ", activeB->board[col][row]);
        } printf("\n");
    } printf("\n");
}

void addboard(Node *tmpN, Qu *queue){
    tmpN->parent = queue->headQ;
    tmpN->next = NULL;
    queue->tailQ->next = tmpN;
    queue->tailQ = tmpN;
    //printboard(tmpN);
}

void printsol(Node *lastBoard){
    int row = 0, col = 0;
    //printf("Solution\n");
    if (lastBoard != NULL){
        for (row = 0; row < 8; row++){
            for (col = 0; col < 7; col++){
                printf("%c ", lastBoard->board[col][row]);
            } printf("\n");
        } printf("\n");
    }
    
    else exit(0);
    printsol(lastBoard->parent);
}

void playboard(Node *boardN, int x, int y){
    int col, row, headN = 0;//, bn=0;
    Node *tmpN;
    Qu *queue;
    tmpN = (Node *)malloc(sizeof(Node));
    queue = (Qu *)malloc(sizeof(Qu));
    queue->headQ = queue->tailQ = boardN;
    while (queue->headQ->board[x][y] != 'O')    //Used to stop playing the game
    {
        //printboard(queue->headQ);
        for (row = 0; row < 8; row++){
            for (col = 0; col < 7; col++){
                if (queue->headQ->board[col][row] == 'O'){
                    //printf("\nRow: %d, Col: %d = %c", row, col, queue->headQ->board[row][col]);
                    /*Top search*/
                    if (row > 1){
                        if (queue->headQ->board[col][row - 1] == 'O' && 
                                queue->headQ->board[col][row - 2] == '.'){
                            printf("\nTop move made");
                            printf("\nRow: %d, Col: %d\n", row, col);
                            tmpN = queue->headQ;
                            tmpN->board[col][row] = '.';
                            tmpN->board[col][row - 1] = '.';
                            tmpN->board[col][row - 2] = 'O';
                            addboard(tmpN, queue);
                            if (queue->headQ->next == NULL && headN == 0)   //To put the second node into the queue
                            {
                                queue->headQ->next = tmpN;
                                headN = 1;
                            }
                        }
                    }
                    /*Right search*/
                    if (col < 5){
                        if (queue->headQ->board[col + 1][row] == 'O' && 
                                queue->headQ->board[col + 2][row] == '.'){
                            printf("\nRight move made");
                            printf("\nRow: %d, Col: %d\n", row, col);
                            tmpN = queue->headQ;
                            tmpN->board[col][row] = '.';
                            tmpN->board[col + 1][row] = '.';
                            tmpN->board[col + 2][row] = 'O';
                            addboard(tmpN, queue);
                            if (queue->headQ->next == NULL && headN == 0)   //To put the second node into the queue
                            {
                                queue->headQ->next = queue->tailQ;
                                headN = 1;
                            }
                        }
                    }
                    /*Bottom search*/
                    if (row < 6)
                    {
                        if (queue->headQ->board[col][row + 1] == 'O' && queue->headQ->board[col][row + 2] == '.')
                        {
                            printf("\nBottom move made");
                            printf("\nRow: %d, Col: %d\n", row, col);
                            tmpN = queue->headQ;
                            tmpN->board[col][row] = '.';
                            tmpN->board[col][row + 1] = '.';
                            tmpN->board[col][row + 2] = 'O';
                            addboard(tmpN, queue);
                            if (queue->headQ->next == NULL && headN == 0)   //To put the second node into the queue
                            {
                                queue->headQ->next = queue->tailQ;
                                headN = 1;
                            }
                        }
                    }
                    /*Left search*/
                    if (col > 1)
                    {
                        if (queue->headQ->board[col - 1][row] == 'O' && queue->headQ->board[col - 2][row] == '.')
                        {
                            printf("\nLeft move made");
                            printf("\nRow: %d, Col: %d\n", row, col);
                            tmpN = queue->headQ;
                            tmpN->board[col][row] = '.';
                            tmpN->board[col - 1][row] = '.';
                            tmpN->board[col - 2][row] = 'O';
                            addboard(tmpN, queue);
                            if (queue->headQ->next == NULL && headN == 0)   //To put the second node into the queue
                            {
                                queue->headQ->next = queue->tailQ;
                                headN = 1;
                            }
                        }
                    }
                }
            }
        } row = col = 0;
        queue->headQ = queue->headQ->next;  //Move to the next board
        if (queue->headQ->board[x][y] == 'O')
        {
            printsol(queue->headQ);
            exit(0);
        }
        else
        {
            printf("\nNo solution can be found\n");
            exit(0);
        }
    }
}