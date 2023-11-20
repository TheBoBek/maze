#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Definice hranic
#define LEFT_BORDER 0x1   // 001 v binárním tvaru
#define RIGHT_BORDER 0x2  // 010 v binárním tvaru
#define TOP_BOTTOM_BORDER 0x4 // 100 v binárním tvaru

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

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
    printf("(%dx%d) ", r, c);
    return ((cell - '0') & border) != 0;
}
void free_map(Map *map) {
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
        // TODO: Combine both conditions
        // If row even and column odd // If row odd column even
        if ((row % 2 == 0 && column % 2 == 1) || (row % 2 == 1 && column % 2 == 0)){
            if (!(isborder(map, row, column, TOP_BOTTOM_BORDER)
                    == isborder(map,row + 1, column, TOP_BOTTOM_BORDER))){
                printf("Fail1");
                return false;
            }
        }
    }

    printf("|");
    // If cell is NOT on the last column, checks for right border
    if (column < map->cols - 1){
        if (!(isborder(map, row, column, RIGHT_BORDER)
        == isborder(map,row, column + 1, LEFT_BORDER))){
            printf("Fail2");
            return false;
        }
    }
    printf("|");
    return true;

}
// Checks if the map is valid
bool isMapValid(Map *map){
    int counter = 0;
    while (counter < (map->rows * map->cols)) {
        // Checks for valid data, if invalid, returns false
        char cell = map->cells[counter];
        if ((isdigit(cell) && (cell - '0') < 8 && (cell - '0') > -1) || cell == '\n' || cell == ' '){
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

    return true;
}

int main(int argc, char *argv[]){
    if (argc > 1){
        if (strcmp(argv[1],"--help") == 0){
            printf("Napoveda\n");
        }
    }
    Map myMap;

    myMap.rows = 6;
    myMap.cols = 7;

    if (!init_map(argv[argc - 1],&myMap)) {
        return 1;  // Chyba při inicializaci mapy
    }

    if (!isMapValid(&myMap)){
        return 1;
    }



    // bool hasLeftBorder = isborder(&myMap, 5, 0, LEFT_BORDER);
    // printf("Left border at: %s\n", hasLeftBorder ? "Yes" : "No");
    // // Zde můžete pracovat s mapou...

    free_map(&myMap);
    return 0;
}