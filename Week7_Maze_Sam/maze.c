/* 	RECURSIVE MAZE ESCAPE (BASIC + SDL + !RANDOM MAZE GENERATION!)

	Please note:	Random maze generation suffers an intermittent seg fault
					which I couldn't find :( BASIC and SDL are unaffected.

	- User input of a maze file via argv[1]
	- graphical output of the solution can be turned on via argv[2] == "SDL"
	- Maze is no larger than 20x20 by design
	- Walls are indicated with # and paths are spaces 
	- Entrance is the gap in the wall closest to top left hand corner
	- Size of each maze is given on the first line of the file (width, height)

	NB: If graphical output is turned on the command line will continue to run in a reduced mode
		Successful maze escapes will not be printed to the command line, but user messages will.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "neillsdl2.h"

#define MAX_SIZE 20
#define MILLISECONDDELAY 300
#define RADIUS 7
#define OUT_OF_BOUNDS 21
#define BASECASE 2
#define EDGE_OFFSET 3

struct maze_entrance {
	int x;
	int y; // Using y positive downwards (South)
}; 

struct maze_dimensions {
	int width; 	// Corresponds to j (cols) in array[i][j]
	int height; // Corresponds to i (rows)
}; 

typedef struct maze_entrance maze_entrance;
typedef struct maze_dimensions maze_dimensions;
typedef char max_maze[MAX_SIZE][MAX_SIZE];

/* ------------ PROTOTYPES -------------- */

int explore(int x, int y, max_maze array, maze_dimensions dims);
int onEdge(int x, int y, maze_dimensions dims);
int scanCheck(int a, int b);
int initialise_SDL(char *argv[]);
int chamber_divide(max_maze array, maze_dimensions maze_dims, maze_dimensions c_dims, int tl_i, int tl_j);

void display_Terminal(max_maze array, maze_dimensions dims);
void display_SDL(max_maze array, maze_dimensions dims);
void check_dimensions(int given_w, int counted_w, int given_h,int counted_h);
void initialiseArray(max_maze array, int size);
void random_maze(max_maze array, maze_dimensions dims);
void open_portals(max_maze array,maze_dimensions dims);
void arg_check(int arg);
void create_walls(max_maze array, maze_dimensions c_dims,int tl_i,int tl_j,int div_i,int div_j);
void create_doors(max_maze array, maze_dimensions maze_dims, int tl_i, int tl_j, int div_i, int div_j);

maze_dimensions find_chamber_dims(max_maze array, int tl_i,int tl_j);
maze_entrance findEntrance(max_maze array, maze_dimensions dims);
maze_dimensions readMaze(char *argv[], max_maze maze);
maze_dimensions random_dimensions();
maze_dimensions get_maze(char *argv[], max_maze array);


/* ------------ MAIN ---------------------- */

int main(int argc, char *argv[])
{	
	srand(time(NULL));
	int SDL = 0; 				// SDL flag
	maze_entrance entrance; 	// Stores coords of entrance to maze
	maze_dimensions dimensions; // Stores width and height of the maze as given in file
	max_maze mazeArray; 		// Char array of maximum size
	
	arg_check(argc);

	// Initalise to spaces. In case the user has entered a newline early.
	initialiseArray(mazeArray, MAX_SIZE); 

	// Inspect and act on argv[1]. Get maze from file or generate it.
	dimensions = get_maze(argv, mazeArray);

	// If argv[2] is given, set/don't set graphics flag. Will ignore further arguements.
	if (argc >= 3) {
		SDL = initialise_SDL(argv);
	}
	
	entrance = findEntrance(mazeArray,dimensions);

	if(explore(entrance.x, entrance.y, mazeArray, dimensions)){
		
		printf("Route found!\n\n");

		if(SDL){
			display_SDL(mazeArray, dimensions); // Display solution with SDL GUI
		}
		else {
			display_Terminal(mazeArray, dimensions); // Use the terminal unless told otherwise
		}
		return 0;
	} 
	else { 
		// If unsuccessful, show the scanned maze in terminal regardless of SDL flag:
		printf("No path found through the maze!\nThe maze was scanned as:\n");
		display_Terminal(mazeArray, dimensions);
		
		if(SDL){
			display_SDL(mazeArray, dimensions);
		}

		return 1;
	}
}


/* ------------------- FUNCTIONS ------------------------ */

/* Self-referencing function that explores the maze recursively returning 1/0 on success/failure */
int explore(int x, int y, max_maze array, maze_dimensions dims){	

	// Bounds check (avoid seg fault)
	if (x >= dims.width || y >= dims.height || y < 0 || x < 0){
		return 0;
	}

	// Base case: cell is THE exit
	if ( onEdge(x,y,dims) && array[y][x] == ' '){	
		array[y][x]='.';
		return 1;
	}

	// Base case: cell is wall OR the cell we just came from
	if (array[y][x] == '#' || array[y][x] == '.'){
		return 0;
	}

	// Cell is neither exit nor wall. Drop a breadcrumb and keep going.
	array[y][x] = '.'; 

	// Try going EAST-WEST-NORTH-SOUTH...
	if (explore(x+1, y, array, dims)) {return 1;}
	if (explore(x-1, y, array, dims)) {return 1;}
	if (explore(x, y-1, array, dims)) {return 1;}
	if (explore(x, y+1, array, dims)) {return 1;}

	// Dead ends all around us, pick up the breadcrumb and return 0
	array[y][x] = ' ';
	return 0;
}



/* Collect maze from file or randomly generate it. Return the maze dimensions. */
maze_dimensions get_maze(char *argv[], max_maze array){

	maze_dimensions dimensions;

	// Inspect and act on argv[1]
	if (strcmp(argv[1],"RANDOM") == 0) {
		dimensions = random_dimensions();  // Decide how big the random maze will be
		random_maze(array,dimensions);	// Fill that portion of the array with a maze
		open_portals(array, dimensions);
	}
	else {
		// Read contents of file into mazeArray, and return given size
		dimensions = readMaze(argv,array);
	}
	return dimensions;
}



/* Return 1/0 depending on whether a cell x,y j,i lies on any edge of an array or not */
int onEdge(int x, int y, maze_dimensions dims){

	if (x == 0 || y == 0 || x == dims.width - 1 || y == dims.height - 1)
	{
		return 1;
	} 
	else {
		return 0;
	}
}



/* Compare two integers, useful for scan checking */
int scanCheck(int a, int b){
	if (a == b) {
		return 1;
	}
	else {
		return 0;
	}
}



/* Check the second command line argument */
int initialise_SDL(char *argv[]){

	// Initialise SDL if given as command line argv2
	if (strcmp(argv[2],"SDL") == 0) {
		printf("\nGraphical output initialised\n");
		return 1;
	} 
	else {	
		printf("\nGraphical output not initialised\nDid you mean 'SDL' ?\n");
		return 0;
	}
}



/* Print portion of 2D character array to screen (will automatically add newlines) */
void display_Terminal(max_maze array, maze_dimensions dims){

	for (int i = 0; i < dims.height; ++i) {
		for (int j = 0; j < dims.width; ++j) {
			printf("%c", array[i][j]);
		} printf("\n");
	} 

	printf("\n");
}



/* Display the maze in an SDL GUI */
void display_SDL(max_maze array, maze_dimensions dims){
	
	// Fit the maze to the window height by scaling each cell's dimensions
	int rectsize = WHEIGHT / dims.height; 
	int window_offset = (WWIDTH-WHEIGHT)/2;
	int cx=0, cy =0;
	
	SDL_Simplewin win;
	SDL_Rect rectangle;
	rectangle.w = rectsize;
	rectangle.h = rectsize;
	Neill_SDL_Init(&win);

	do {
		// Draw the array graphically.
		for (int i = 0; i < dims.height; ++i) {
			for (int j = 0; j < dims.width; ++j) {

				if ( array[i][j] == '#' ) {
					Neill_SDL_SetDrawColour(&win,255,255,255); // Set colour to WHITE
					
					// Draw rectangle for each array cell, centred in the window
					rectangle.x = (j * rectsize) + window_offset ;
					rectangle.y = i * rectsize;
					SDL_RenderFillRect(win.renderer, &rectangle);
				} 
				if ( array[i][j] == '.') {	
					Neill_SDL_SetDrawColour(&win,255,128,0); // Set colour to ORANGE

					cx = (j*rectsize) + window_offset + (rectsize/2);
					cy = (i*rectsize) + (rectsize/2);
					Neill_SDL_RenderFillCircle(win.renderer, cx, cy, RADIUS);
				}
			}
		}

		// Update window.
		SDL_RenderPresent(win.renderer);
		SDL_UpdateWindowSurface(win.win);
		Neill_SDL_Events(&win); 		// Check for user kill

		SDL_Delay(MILLISECONDDELAY); 	// Wait a short time

	} while(!win.finished);

	// Cleans up.
	atexit(SDL_Quit);
}



/* Print warning if given and measured dimensions don't match */
void check_dimensions(int given_w, int counted_w, int given_h,int counted_h){

	if (counted_w != given_w || counted_h != given_h) 
	{
		printf("\nWARNING: The given dimensions don't match the maze! Maybe the file finishes with a newline?\n");
		printf("Maze dimensions measured as: %d x %d\n", counted_w, counted_h);
	}
}



/* Initialise a 2D array to blank spaces */
void initialiseArray(max_maze array, int size){

	for (int i = 0; i < size; ++i){
		for (int j = 0; j < size; ++j){
			array[i][j] = ' ';
		}
	}
}



/* Scan file for characters and put them into a portion of a max-sized array, 
also returns maze dimensions as given on the file topline while checking them */
maze_dimensions readMaze(char *argv[], max_maze array){

	char c;
	int i = 0, j = 0, i_max = 0, j_max = 0, check = 0;
	maze_dimensions dims;
	FILE *mazefile;
	mazefile = fopen(argv[1],"r");

	if (mazefile == NULL) {
		printf("Error opening file.\n"); 
		exit(1);
	}
	else {
		check = fscanf(mazefile, "%d %d\n",&dims.width,&dims.height); // Scan the dimensions from topline
		if(scanCheck(check, 2) != 1){
			printf("Given dimensions are invalid.\n");
		}

		// Get the next character in the file until we run out:
		while((c = fgetc(mazefile)) != EOF) {
		
			if (c == '\n') { 		// If you find a newline, go to the next row down
				j=0;
				i++;
				
				if (i_max < i) {	// Store the largest values of i and j to check dimensions
					i_max = i;
				}
			}
				
			else { 					// Work along the current row
				array[i][j]=c;
				j++;

				if (j_max < j) {
					j_max = j;
				}
			}
		}	
		fclose(mazefile);
	} 

	check_dimensions(dims.width,j_max, dims.height, i_max + 1);
	//i_max counts number of rows, but from zero. j_max does this also, but is incremented once extra.
	printf("\nMaze dimensions given as: %d x %d\n", dims.width, dims.height);

	return dims;
}




/* Return coords of opening in maze nearest to 0,0 */
maze_entrance findEntrance(max_maze array, maze_dimensions dims){

	int dist, ent_candidate, i, j;
	maze_entrance ent;

	ent.x = OUT_OF_BOUNDS; // Initialise to some impossible coord
	ent.y = OUT_OF_BOUNDS;

	ent_candidate = dims.width + dims.height; // Furthest point (worst case)

	for (i = 0; i < dims.height; ++i){
		for (j = 0; j < dims.width; ++j){
			// if cell is on edge, and empty:
			if( onEdge( j, i, dims) && array[i][j] == ' '){
				dist = i + j; // Sum of coords used as metric for distance to origin
				
				if (dist < ent_candidate){
					ent_candidate = dist; 
					ent.y = i;
					ent.x = j;
				}	 
			}
		}
	}

	// If the coords haven't been found
	if (ent.x == OUT_OF_BOUNDS || ent.y == OUT_OF_BOUNDS) {
		printf("No entrance found!\n");
		display_Terminal(array, dims);
		exit(1);
	} else {
		array[ent.y][ent.x] = 'E';	// Mark with an E to distinguish from the exit/goal
	}

	return ent;
}




/* Add at least one entrance and exit in case none were created. Strip all '*' placeholders */
void open_portals(max_maze array,maze_dimensions dims){
	for (int i = 0; i < dims.height; ++i)
	{
		for (int j = 0; j < dims.width; ++j)
		{
			if (array[i][j] == '*')
			{
				array[i][j] = ' ';
			}
		}
	}

	// Harcoded entrance (near top left)
	array[0][1] = ' ';
	// Hardcoded exit (near bottom right)
	array[dims.height - 1][dims.width - 2] = ' ';
}



/* Make sure user has entered at least one argument */
void arg_check(int arg){
	if (arg == 1)
	{
		printf("No arguments entered. Please enter a filename or 'RANDOM'\n");
		exit(1);
	}
}



/* Random maze generation. */
void random_maze(max_maze array, maze_dimensions dims){

	// Need initial set of dummy dimensions to pass to divide func:
	maze_dimensions c_dims; 

	// Border the maze with a wall before beginning
	for (int i = 0; i < dims.width; ++i){
		for (int j = 0; j < dims.height; ++j){
			if(onEdge( j, i, dims)){
				array[i][j] = '#';
			}
		}
	}
	
	// Divide the remaining chamber in four
	if( chamber_divide(array, dims, c_dims, 1, 1) ){
		printf("Generation finished\n");
	}
}


/* Divide the chamber recursively. Necessary parameters:
	- the array in which the maze exists 
	- the size of the whole maze (for printing)
	- the size of the current 'sector' of the maze
	- an absolute reference point for the sector's position in the array:
		- the coords of top-left corner

	NB: THIS IS THE FUNCTION THAT SEG FAULTS. But it's close... 
 */
int chamber_divide(max_maze array, maze_dimensions maze_dims, maze_dimensions c_dims, int tl_i, int tl_j){

	int div_j = 0, div_i = 0;	// Indices of dividing walls in array
	c_dims.height = 0;	
	c_dims.width = 0;

	// How big is the current empty chamber?
	c_dims = find_chamber_dims(array, tl_i, tl_j);

	// BASE CASE: If the chamber is only TWO cells wide OR tall, stop dividing
	if (c_dims.width <= BASECASE || c_dims.height <= BASECASE) {
		return 0; 
	}

	//If both h+w are greater than three cells, split it in four:

	/* Note: To create random numbers for an offset that places the division between 
	near edge + 1 and far edge - 1 we have to do %(something - 3). So must carefully 
	handle cases where 'something' is near 3 (EDGE_OFFSET): */

	else if (c_dims.width > EDGE_OFFSET && c_dims.height > EDGE_OFFSET) {

		int randw_offset = 1 + rand()%(c_dims.width - EDGE_OFFSET);
		int randh_offset = 1 + rand()%(c_dims.height - EDGE_OFFSET);

		div_j = tl_j + randw_offset; 
		div_i = tl_i + randh_offset;
	}

	else if(c_dims.width == EDGE_OFFSET || c_dims.height == EDGE_OFFSET){
		div_i = tl_i + 1;
		div_j = tl_j + 1;
	} 

	// Create the new walls
	create_walls(array, c_dims, tl_i, tl_j,div_i,div_j);

	// Create the doors (only with *'s for now)
	create_doors(array, maze_dims,tl_i, tl_j, div_i, div_j);

	// Once split, recurse down into the new, smaller sector. Then move on the next clockwise sector
	chamber_divide(array, maze_dims, c_dims, tl_i, tl_j ); // Try the top left chamber
	chamber_divide(array, maze_dims, c_dims, tl_i, div_j+1); // Try the top right
	chamber_divide(array, maze_dims, c_dims, div_i+1, div_j+1); // Try the bottom right
	chamber_divide(array, maze_dims, c_dims, div_i+1, tl_j); // Try the bottom left

	return 1;
}



/* Put a hole in 3 of the four dividing walls */
void create_doors(max_maze array, maze_dimensions maze_dims, int tl_i, int tl_j, int div_i, int div_j){
	
	int i, j;

	// Choose one wall of the four not to get a door/gap in it
	int noDoor = rand() % 4; 

	if (noDoor != 0) // Upright, top half.
	{	
		i = tl_i + rand()%(div_i - tl_i);
		array[i][div_j] = '*'; 
	} // Mark the door with an asterisk for now, will be stripped later.

	if (noDoor != 1) // Upright, bottom half.
	{	
		i = tl_i + div_i + rand()%(maze_dims.height - div_i);
		array[i][div_j] = '*';
	}

	if (noDoor != 2) // Horizontal, left half.
	{	
		j = tl_j + rand()%(div_j - tl_j);
		array[div_i][j] = '*';
	}

	if (noDoor != 3) // Horizontal, right half.
	{	
		j = tl_j + div_j + rand()%(maze_dims.width - div_j);
		array[div_i][j] = '*';
	}
}


/* Return some random, square maze dimensions */
maze_dimensions random_dimensions(){

	maze_dimensions dims;

									// Decide the random size of the maze
	dims.width = 5 + rand() % 16; 	// Random number between 5 and 20. ( 5 + 0->15 )
	dims.height = dims.width;

	printf("\nRANDOM MODE: Maze will be %d by %d\n", dims.width, dims.height);

	return dims;
}



/* Create the new walls during division */
void create_walls(max_maze array, maze_dimensions dims, int tl_i, int tl_j, int div_i, int div_j){

	for (int i = tl_i; i < tl_i + dims.height; ++i)
	{
		for (int j = tl_j; j < tl_j + dims.width; ++j)
		{
			array[i][div_j] = '#';
			array[div_i][j] = '#';
		}
	}
}


/* Find the height and width of the current chamber/sector. Excludes walls. */
maze_dimensions find_chamber_dims(max_maze array, int tl_i,int tl_j){
	
	maze_dimensions c_dims;
	int i = tl_i; 		// "top-left-i" index. location of top-left cell in chamber.
	int j = tl_j; 		// "top-left-j"
	char c = 'c';
	
	c_dims.height = 0;	
	c_dims.width = 0;

	// Starting at top-left, while cell is neither wall # nor door *:
	while( c!='#' && c!= '*'){
		i++;
		c_dims.height++;
		c = array[i][tl_j];
	}
	
	c='c'; // Reset c
	while(c!='#' && c!= '*'){
		j++;
		c_dims.width++;
		c = array[tl_i][j];
	}
	return c_dims;
}

