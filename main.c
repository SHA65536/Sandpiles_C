#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <unistd.h>
#include <windows.h>

typedef enum {false, true} bool;

#define ANSI_COLOR_WHITE   "\x1b[29m"
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
 
#define length 27
#define middle length/2

void blank_mat(long a[length][length]);
void hidecursor();
void add_mats(long a[length][length], long b[length][length], long sum[length][length]);
void print_mat(long a[length][length], HANDLE Screen);
long is_inside_of_bounds(long x, long y);
long resolve_pixel(long in[length][length], long x, long y);
long step(long in[length][length]);
void resolve_mat(long in[length][length]);
void write_to_file(long name, long mat[length][length]);

// Relative positions of all pixels affecting a center pixel.
const long instructions [8][2] = {
    {-2, -1}, {-2, 1},
    {-1, -2}, {-1, 2},
    {2, -1}, {2, 1},
    {1, -2}, {1, 2}
};

long main(long argc, char *argv[])
{
    long a [length][length];
    bool printToScreen = true;
    bool saveToFile = false;
    int i;
    int iterations = 1000;
	HANDLE Screen = GetStdHandle(STD_OUTPUT_HANDLE);
    int opt;
    while ((opt = getopt(argc, argv, "s::f::i:")) != -1) {
        switch (opt) {
        case 's':  printToScreen=false; break;
        case 'f':  saveToFile=true; break;
        case 'i':  iterations=atoi(optarg); break;
        }
    }
    hidecursor();
    blank_mat(a);

    //Generating each frame.
    for(i=0;i<iterations;i++){
        //Incrementing the middle each frame.
        a[middle][middle]+=1;
        resolve_mat(a);
        printf("%d / %d\r", i, iterations);
        //Printing to 
        if(printToScreen)
            print_mat(a, Screen);
        //Saving the resolved matrix to disk.
        if(saveToFile)
            write_to_file(i, a);
        if(kbhit())
            break;
    }
}

// Stepping the matrix until no changes are made.
void resolve_mat(long in[length][length])
{
    while(step(in)){
        
    }
}

//Topples any piles that are too large
long step(long in[length][length])
{
    long i, j, res, out [length][length];
    long flag = 0;
    //Making a matrix of differences
    for(i=0; i<length; i++){
        for(j=0; j<length; j++){
            out[i][j] = resolve_pixel(in, i, j);
            if (out[i][j]){flag = 1;}
        }
    }
    //Applying differences to matrix
    add_mats(in, out, in);
    //If no changes are made, returns 0 else, returns a positive number
    return flag;
}

//Find sum of differences for a pixel on the grid.
long resolve_pixel(long in[length][length], long x, long y)
{
    long sum = 0, i, dx, dy;
    //Iterating over every relative position.
    for(i=0;i<8;i++)
    {
        dx = instructions[i][0] + x;
        dy = instructions[i][1] + y;
        //Ignore pixels out of bound.
        if(is_inside_of_bounds(dx, dy))
        {
            sum += in[dx][dy] / 8;
        }
    }
    //If the pixel itself is over the toppling threshold, it needs to be emptied.
    if(in[x][y] >= 8)
    {
		sum -= in[x][y] - (in[x][y] % 8);
	}
    return sum;
}

//Generates a matrix full of 0s
void blank_mat(long a[length][length])
{
    long i, j;
    for(i=0; i<length; i++){
        for(j=0; j<length; j++){
            a[i][j] = 0;
        }
    }
}

//Adds to matrixes
void add_mats(long a[length][length], long b[length][length], long sum[length][length])
{
    long i, j;
    for(i=0; i<length; i++){
        for(j=0; j<length; j++){
            sum[i][j] = a[i][j] + b[i][j];
        }
    }
}

long is_inside_of_bounds(long x, long y)
{
    if(x >= length || y >= length){
        return 0;
    }
    if(x < 0 || y < 0){
        return 0;
    }
    return 1;
}

void print_mat(long a[length][length], HANDLE Screen)
{
    long i, j;
	static COORD Position = {0,0};
	SetConsoleCursorPosition(Screen, Position);
    for(i=0; i<length; i++){
        for(j=0; j<length; j++){
            switch(a[i][j]){
                case 0:
                    printf(ANSI_COLOR_BLACK    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 1:
                    printf(ANSI_COLOR_WHITE    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 2:
                    printf(ANSI_COLOR_BLUE     "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 3:
                    printf(ANSI_COLOR_CYAN     "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 4:
                    printf(ANSI_COLOR_GREEN    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 5:
                    printf(ANSI_COLOR_MAGENTA    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 6:
                    printf(ANSI_COLOR_YELLOW    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
                case 7:
                    printf(ANSI_COLOR_RED    "%c%c"     ANSI_COLOR_RESET, (char)219u, (char)219u);
                    break;
            }
        }
        printf("\n");
    }
    printf("Press any key to exit.\n");
}

//Writing the matrix to a file, which can be later turned into an image.
void write_to_file(long name, long mat[length][length])
{
    // creating file pointer to work with files
    char *name_str = (char*)malloc(17 * sizeof(char));
    long i, j;
    sprintf(name_str, "output/%06d.txt", name);

    FILE *fptr;

    // opening file in writing mode
    fptr = fopen(name_str, "w");

    for(i=0; i<length; i++){
        for(j=0; j<length; j++){
            fprintf(fptr, "%d ", mat[i][j]);
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
    free(name_str);
    
}

//Hides the Curosr
void hidecursor()
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}