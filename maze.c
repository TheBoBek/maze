#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Definice hranic
#define LEFT_BORDER 0x1   // 001 v binárním tvaru
#define RIGHT_BORDER 0x2  // 010 v binárním tvaru
#define TOP_BOTTOM_BORDER 0x4 // 100 v binárním tvaru

#define RIGHT_DIRECTION 0
#define TOP_DIRECTION 1
#define LEFT_DIRECTION 2
#define BOTTOM_DIRECTION 3

#define LEFT_RULE 0
#define RIGHT_RULE 1

// TODOs:
// 1) Test if NO ENTRY is working

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;
typedef struct{
    int row;
    int col;
}Cell;
bool init_map(const char *filename, Map *map) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    if (fscanf(file, "%d %d", &map->rows, &map->cols) != 2) {
        fprintf(stderr, "Error reading map dimensions\n");
        fclose(file);
        return false;
    }

    //Initialization of row and cols
    map->cells = malloc(map->rows * map->cols * sizeof(unsigned char));
    if (map->cells == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return false;
    }
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->cols; j++) {
            int cell;
            if (fscanf(file, "%d", &cell) != 1) {
                fprintf(stderr, "Error reading map data\n");
                free(map->cells);
                fclose(file);
                return false;
            }
            map->cells[i * map->cols + j] = (unsigned char)cell + '0';
        }
    }

    fclose(file);
    return true;
}

bool isborder(Map *map, int r, int c, int border) {
    if (map == NULL || r < 0 || r >= map->rows || c < 0 || c >= map->cols) {
        if (c < 0){
            printf("BUG");
        }
        fprintf(stderr, "Invalid map coordinates2\n");
        return false;
    }
    unsigned char cell = map->cells[r * map->cols + c];
    // printf("(%dx%d) ", r, c);
    return ((cell - '0') & border) != 0;
}
void mapDtor(Map *map) {
    free(map->cells);
    map->cells = NULL;
}

//TODO Dokonci funkciu
// Function checks if cell borders are valid with other cells
bool shareCellsBorder(Map *map, int counter){
    // TODO: Differentiate parne/neparne
    int row = counter / map->cols;
    int column = counter % map->cols;
    // printf("Row:%d Column:%d |\n", row, column);
    // If cell is NOT on last row, checks for top or bottom border
    if (!((counter > map->cols * (map->rows - 1)))){
        // If row even and column odd // If row odd column even
        if ((row % 2 == 0 && column % 2 == 1) || (row % 2 == 1 && column % 2 == 0)){
            if (!(isborder(map, row, column, TOP_BOTTOM_BORDER)
                    == isborder(map,row + 1, column, TOP_BOTTOM_BORDER))){
                printf("Fail1");
                return false;
            }
        }
    }

    // If cell is NOT on the last column, checks for right border
    if (column < map->cols - 1){
        if (!(isborder(map, row, column, RIGHT_BORDER)
        == isborder(map,row, column + 1, LEFT_BORDER))){
            printf("Fail2");
            return false;
        }
    }
    return true;

}
// Function cecks if the cell is entry cell (If there is outer border in maze)
void isEntryCell(Map *map, Cell *cell, int row, int column){
    printf("(%d %d)", row, column);
    // Check first row
    if(row == 0 && column % 2 == 0 && !isborder(map, row, column, TOP_BOTTOM_BORDER)){
        cell->row = row;
        cell->col = column;
    }
    // Check first column
    else if (column == 0 && !isborder(map, row, column, LEFT_BORDER)){
        cell->row = row;
        cell->col = column;
    }
    // Check last column
    else if (column == map->cols - 1 && !isborder(map, row, column, RIGHT_BORDER)){
        cell->row = row;
        cell->col = column;
    }
    // Check last row
    else if(row == map->rows - 1){
        // printf("Border je: %d\n", !isborder(map, row, column, TOP_BOTTOM_BORDER));
        if (row % 2 == 0 && column % 2 == 1 && !isborder(map, row, column, TOP_BOTTOM_BORDER)){
            cell->row = row;
            cell->col = column;
        }
        else if (row % 2 == 1 && column % 2 == 0 && !isborder(map, row, column, TOP_BOTTOM_BORDER)){
            cell->row = row;
            cell->col = column;
        }
    }
}
// Checks if the map is valid
bool isMapValid(Map *map, Cell *cell_t){
    bool flag = false;
    int counter = 0;
    while (counter < (map->rows * map->cols)) {
        // Checks cells for valid data, if invalid, returns false
        char cell = map->cells[counter];
        if ((isdigit(cell) && (cell - '0') < 8 && (cell - '0') > -1) || cell == '\n' || cell == ' '){
            // Cycle runs only for the row - 1
            if (counter < (map->cols*(map->rows - 1))){
                if (!(shareCellsBorder(map, counter))){
                    printf("Fejl");
                    return false;
                }
                else{
                    printf("Suxes\n");
                }
            }
        }
        else{
            fprintf(stderr, "Invalid map data\n");
            return false;
        }
        counter++;
    }
    // TODO: Dokoncit
    // Checks if rows and columns are valid
    if (counter != map->rows * map->cols){
        printf("Inavlid map dimensions");
        return false;
    }
    if (!flag){
        printf("Entry cell not found\n");
        return false;
    }
    else{
        printf("Entry cell is on position: (%d %d)", cell_t->row, cell_t->col);
    }

    return true;
}

// bool findExitCell(Map *map, Cell *cell){
//     int column = 0;
//     for (column < map->cols; column++){
//         if (isExitCell(map, cell, 0, column))
//         {
//             /* code */
//         }
        
//     }
// }
int start_border(Map *map, int r, int c, int leftright){
    printf("%d a %d",r ,c);
    // If RIGHT rule
    if (leftright == 1){
        // 1) RIGHT: entry is from LEFT on even row !!! counting for 'r' starts from 0 !!!
        if (r % 2 == 1 && c == 0){
            return BOTTOM_DIRECTION;
        }
        // 2) BOTTOM: entry is LEFT on odd row !!! counting for 'r' starts from 0 !!!
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
            else{
                return LEFT_DIRECTION;
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
            printf("Bug\n");
            return -1;
        }
    }
    // If LEFT rule
    return false;

}

int move(Map *map, Cell *position, int dir){
    printf("\nSMER JE: %d a ", dir);
    dir = (4 + dir - 1) % 4;
    printf("%d", dir);
    
    for (int i = 0; i < 4; i++){
        printf("\nSmer je: %d a suradnice su: (%dx%d)", dir, position->row, position->col);
        if (dir == RIGHT_DIRECTION){
            if (!isborder(map, position->row, position->col, RIGHT_BORDER)){
                position->col++;
                return dir;
            }
        }
        else if (dir == TOP_DIRECTION && ((position->row % 2 == 0 && position->col % 2 == 0) ||
                                          (position->row % 2 == 1 && position->col % 2 == 1))){
            if (!isborder(map, position->row, position->col, TOP_BOTTOM_BORDER)){
                position->row--;
                return dir;
            }
        }
        else if (dir == LEFT_DIRECTION){
            if (!isborder(map, position->row, position->col, LEFT_BORDER)){
                position->col--;
                return dir;
            }
        }
        else if (dir == BOTTOM_DIRECTION && ((position->row % 2 == 0 && position->col % 2 == 1) || 
                                             (position->row % 2 == 1 && position->col % 2 == 0))){
            if (!isborder(map, position->row, position->col, TOP_BOTTOM_BORDER)){
                position->row++;
                return BOTTOM_DIRECTION;
            }
        }
        dir = (dir + 1) % 4;
    }
    printf("BUG");
    return false;
}

int main(int argc, char *argv[]){
    if (argc > 1){
        if (strcmp(argv[1],"--help") == 0){
            printf("Napoveda\n");
        }
    }
    Map myMap;
    Cell cellPosition;
    Cell exitCell;

    cellPosition.row = *argv[2] - '0' - 1;
    cellPosition.col = *argv[3] - '0' - 1;

    exitCell.row = 0;
    exitCell.col = 2;

    if (!init_map(argv[argc - 1],&myMap)) {
        return 1;  // Chyba při inicializaci mapy
    }

    // if (!isMapValid(&myMap, &entryCell)){
    //     return 1;
    // }

    int rule;


    if (strcmp(argv[1], "--lpath") == 0){
        rule = LEFT_RULE;
    }
    else if(strcmp(argv[1], "--rpath") == 0){
        rule = RIGHT_RULE;
    }
    else {
        fprintf(stderr, "Invalid input\n");
    }    
    int dir;
    dir = start_border(&myMap, cellPosition.row, cellPosition.col, rule) + 1;
    printf("Smer je: %d\n", dir);
    int counter = 0;
    // cellPosition.row != exitCell.row || cellPosition.col != exitCell.col || 
    while (counter < 50 && (cellPosition.row != exitCell.row || cellPosition.col != exitCell.col)){
        // Toto je dobre
        dir = move(&myMap, &cellPosition, dir);
         
        // Safety break
        counter++;
    }
    



    // bool hasLeftBorder = isborder(&myMap, 5, 0, LEFT_BORDER);
    // printf("Left border at: %s\n", hasLeftBorder ? "Yes" : "No");
    // // Zde můžete pracovat s mapou...

    mapDtor(&myMap);
    return 0;
}