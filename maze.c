// Šimon Bobko
// xbobkos00

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Border definitions
#define LEFT_BORDER 1
#define RIGHT_BORDER 2
#define TOP_BOTTOM_BORDER 4

// Direction definitions
#define RIGHT_DIRECTION 0
#define TOP_DIRECTION 1
#define LEFT_DIRECTION 2
#define BOTTOM_DIRECTION 3

// Rules definitions
#define LEFT_RULE -1
#define RIGHT_RULE 1

// Number of directions
#define NUM_DIRS 4


typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

typedef struct{
    int row;
    int col;
}Cell;

// Load map data from file to structure Map
void load_map(Map *map, FILE *file){
    int i = 0;
    int cell;
    // Appendation of map data
    while (fscanf(file, "%d", &cell) == 1 && i < map->cols*map->rows){
        map->cells[i] = (unsigned char)cell + '0';
        i++;
    }
    map->cells[i] = '\0';
}

// Function initializes map from specified file
bool init_map(const char *filename, Map *map) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    // Read header of the file with map dimensions
    if (fscanf(file, "%d %d", &map->rows, &map->cols) != 2) {
        fprintf(stderr, "Error reading map dimensions\n");
        fclose(file);
        return false;
    }

    //Initialization of rows and cols
    map->cells = malloc(map->rows * map->cols * sizeof(unsigned char) + 1);
    if (map->cells == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return false;
    }

    load_map(map, file);

    fclose(file);
    return true;
}

// Function checks if cell has specific border
bool isborder(Map *map, int r, int c, int border) {
    if (map == NULL || r < 0 || r >= map->rows || c < 0 || c >= map->cols) {
        if (c < 0){
        }
        fprintf(stderr, "Invalid map coordinates\n");
        return false;
    }
    // Select cell from map
    unsigned char cell = map->cells[r * map->cols + c];

    // Based on binary values checks if is border
    if (((cell - '0') & border) != 0){
        return true;
    }
    else{
        return false;
    }
}

// Destruction of struct
void mapDtor(Map *map) {
    free(map->cells);
    map->cells = NULL;
}

// Function checks if cell borders are valid with other cells
bool shareCellsBorder(Map *map, int counter){
    int row = counter / map->cols;
    int column = counter % map->cols;
    // If cell is NOT on last row, checks for top or bottom border
    if (!((counter > map->cols * (map->rows - 1)))){
        // If row odd and column even || If row even column odd
        if ((row % 2 == 0 && column % 2 == 1) || (row % 2 == 1 && column % 2 == 0)){
            if (!(isborder(map, row, column, TOP_BOTTOM_BORDER)
               == isborder(map,row + 1, column, TOP_BOTTOM_BORDER))){
                return false;
            }
        }
    }
    // If cell is NOT on the last column, checks for right border
    if (column < map->cols - 1){
        if (!(isborder(map, row, column, RIGHT_BORDER)
           == isborder(map,row, column + 1, LEFT_BORDER))){
            return false;
        }
    }
    return true;
}

// Function checks if the map is valid
bool isMapValid(Map *map){
    if (map->cells < 0 || map->rows < 0){
        return false;
    }
    int i = 0;
    // Counts cells 
    while (map->cells[i] != '\0'){
        i++;
    }
    // Validation of cells count
    if (i != map->rows * map->cols){
        return false;
    }
    
    int counter = 0;
    while (counter < (map->rows * map->cols)) {
        // Checks cells for valid data, if invalid, returns false
        char cell = map->cells[counter];
        if ((isdigit(cell) && (cell - '0') < 8 && (cell - '0') > -1) || cell == '\n' || cell == ' '){
            // Cycle runs only for the row - 1
            if (counter < (map->cols*(map->rows - 1))){
                if (!(shareCellsBorder(map, counter))){
                    return false;
                }
            }
        }
        else{
            fprintf(stderr, "Invalid map data\n");
            return false;
        }
        counter++;
    }
    return true;
}

// Function decides which direction will cell follow, returns direction (int) in range from RIGHT to BOTTOM (0-3)
int start_border(Map *map, int r, int c, int leftright){
    // If RIGHT rule
    if (leftright == RIGHT_RULE){
        // 1) BOTTOM: entry is from LEFT on even row !!! counting for 'r' starts from 0 !!!
        if (r % 2 == 1 && c == 0){
            return BOTTOM_DIRECTION;
        }
        // 2) RIGHT: entry is LEFT on odd row !!! counting for 'r' starts from 0 !!!
        else if (r % 2 == 0 && c == 0){
            return RIGHT_DIRECTION;
        }
        // 5, 6) TOP or LEFT: entry is RIGHT, cell has TOP or BOTTOM BORDER
        else if (c == map->cols - 1){
            // 5) TOP: entry is RIGHT, cell has TOP BORDER
            if ((r % 2 == 0 && c % 2 == 0) || (c % 2 == 1 && r % 2 == 1)){
                return TOP_DIRECTION;
            }
            // 6) LEFT: entry RIGHT, cell has BOTTOM BORDER
            else if ((r % 2 == 0 && c % 2 == 1) || (r % 2 == 1 && c % 2 == 0)){
                return LEFT_DIRECTION;
            }
            else{
                fprintf(stderr, "Invalid data\n");
                return -1;
            }
        }
        // 3) LEFT: entry is TOP
        else if (r == 0){
            return LEFT_DIRECTION;
        }
        // 4) RIGHT: entry is BOTTOM
        else if (r == map->rows - 1){
            return RIGHT_DIRECTION;
        }
        else{
            fprintf(stderr, "Error during start border1\n");
            return -1;
        }
    }

    // 1) LEFT: entry is RIGHT odd row
        if (c == map->cols - 1 && r % 2 == 0){
            return LEFT_DIRECTION;
        }
    // 2) BOTTOM: entry is RIGHT even row
        else if (c == map->cols - 1 && r % 2 == 1){
            return BOTTOM_DIRECTION;
        }
    // 5, 6) TOP or RIGHT: entry is LEFT, cell has TOP or BOTTOM BORDER
    // 5) TOP: entry is LEFT, cell has TOP BORDER
        else if (c == 0 && r % 2 == 0 && isborder(map, r, c, TOP_BOTTOM_BORDER)){
            return TOP_DIRECTION;
        }
    // 6) RIGHT: entry is LEFT, cell has BOTTOM BORDER 
        else if (c == 0 && r % 2 == 1 && isborder(map, r, c, TOP_BOTTOM_BORDER)){
            return RIGHT_DIRECTION;
        }
    // 3) RIGHT: entry is TOP
        else if (r == 0){
            if (c % 2 == 1){
                fprintf(stderr, "Error during start border2\n");
                return false;
            }
            return RIGHT_DIRECTION;
        }
    // 4) LEFT: entry is BOTTOM
        else if (r == map->rows - 1){
            if ((r % 2 == 0 && c % 2 == 0) || (r % 2 == 1 && c % 2 == 1)){
                fprintf(stderr, "Invalid entry\n");
            }
            return LEFT_DIRECTION;
        }
        else{
            fprintf(stderr, "Error during start_border3\n");
            return false;
        }
}

// Function moves position of the cell based on previous direction, returns the direction it moved the cell
int move(Map *map, Cell *position, int dir, int leftright){
    // Set new value based on previous direction and leftright rule
    dir = (NUM_DIRS + dir - leftright) % NUM_DIRS;

    // Iterate through all directions possible in triangle
    for (int i = 0; i < NUM_DIRS; i++){
        if (dir == RIGHT_DIRECTION){
            if (!isborder(map, position->row, position->col, RIGHT_BORDER)){
                // Moves position RIGHT
                position->col++;
                return dir;
            }
        }
        else if (dir == TOP_DIRECTION && ((position->row % 2 == 0 && position->col % 2 == 0) ||
                                          (position->row % 2 == 1 && position->col % 2 == 1))){
            if (!isborder(map, position->row, position->col, TOP_BOTTOM_BORDER)){
                // Move position UP
                position->row--;
                return dir;
            }
        }
        else if (dir == LEFT_DIRECTION){
            if (!isborder(map, position->row, position->col, LEFT_BORDER)){
                // Move position LEFT
                position->col--;
                return dir;
            }
        }
        else if (dir == BOTTOM_DIRECTION && ((position->row % 2 == 0 && position->col % 2 == 1) || 
                                             (position->row % 2 == 1 && position->col % 2 == 0))){
            if (!isborder(map, position->row, position->col, TOP_BOTTOM_BORDER)){
                // Move position DOWN 
                position->row++;
                return dir;
            }
        }
        // Set new value for dir based on leftright rule and previous direction
        dir = (NUM_DIRS + dir + leftright) % NUM_DIRS;
    }
    fprintf(stderr, "Error during rotation in cell\n");
    return -1;
}

// Function tests the map
void test(Map *map){
    if (isMapValid(map)){
        printf("Valid\n");
    }
    else {
        printf("Invalid\n");
    }    
}

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "Invalid arguments\n");
        return 0;
    }

    Map myMap;
    Cell cellPosition;
    
    if (!init_map(argv[argc - 1],&myMap)){
            // Error during initialization
            return 1;
    }

    int rule = RIGHT_RULE;
    if (strcmp(argv[1],"--help") == 0){
        printf("Napoveda\n");
    }
    // Map validation
    else if (strcmp(argv[1], "--test") == 0){
        test(&myMap);
        mapDtor(&myMap);
        return 0;
    }
    // Rule and start cell initialization
    else if (strcmp(argv[1], "--lpath") == 0){
        cellPosition.row = *argv[2] - '0' - 1;
        cellPosition.col = *argv[3] - '0' - 1;
        rule = LEFT_RULE;
    }
    else if(strcmp(argv[1], "--rpath") == 0){
        cellPosition.row = *argv[2] - '0' - 1;
        cellPosition.col = *argv[3] - '0' - 1;
        rule = RIGHT_RULE;
    }
    // Error with input
    else {
        fprintf(stderr, "Invalid input\n");
        mapDtor(&myMap);
        return 1;
    }
    
    // Initializing first direction (adds rule constant, which is substracted in move function)
    int dir = start_border(&myMap, cellPosition.row, cellPosition.col, rule) + rule;

    // Cycle runs obeying the rules of rpath or lpath until cellPosition coordinates are out of rows and columns range
    while (!(cellPosition.row < 0 || cellPosition.row >= myMap.rows || cellPosition.col < 0 || cellPosition.col >= myMap.cols)){
        printf("%d,%d\n", cellPosition.row + 1, cellPosition.col + 1);
        dir = move(&myMap, &cellPosition, dir, rule);
        if (dir == -1){
            mapDtor(&myMap);
            return 1;
        }
    }
    mapDtor(&myMap);
    return 0;
}