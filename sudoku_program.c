#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define MAX_SIZE 16
#define SMALL_SIZE 9

// Structure to hold grid configuration
typedef struct {
    int size;           // 9 or 16
    int box_size;       // 3 or 4
    int board[MAX_SIZE][MAX_SIZE];
    // New structure for Killer Sudoku cages
    struct {
        int sum;        // Required sum for the cage
        int cells[4][2]; // Up to 4 cells in the cage (x, y coordinates)
        int cell_count; // Number of cells in the cage
    } cages[MAX_SIZE * MAX_SIZE]; // Maximum number of cages
    int cage_count; // Number of cages
} KillerSudokuGrid;

// Function declarations
void print_board(KillerSudokuGrid* grid);
bool is_valid(KillerSudokuGrid* grid, int row, int col, int num);
bool solve_killer_sudoku(KillerSudokuGrid* grid);
bool init_grid(KillerSudokuGrid* grid, int size);
void generate_killer_puzzle(KillerSudokuGrid* grid, KillerSudokuGrid* solution, int difficulty);
bool check_solution(KillerSudokuGrid* user_grid, KillerSudokuGrid* solution);
bool is_valid_input(KillerSudokuGrid* grid, char input);

// Function to print the Killer Sudoku board
void print_board(KillerSudokuGrid* grid) {
    printf("   ");
    for (int j = 0; j < grid->size; j++) {
        if (j < 10) {
            printf(" %d ", j);
        } else {
            printf(" %c ", 'A' + (j - 10));
        }
        if ((j + 1) % grid->box_size == 0) {
            printf("|"); // Vertical separator for boxes
        }
    }
    printf("\n");

    for (int i = 0; i < grid->size; i++) {
        if ((i % grid->box_size == 0) && (i != 0)) {
            printf("  ");
            for (int j = 0; j < grid->size; j++) {
                if ((j % grid->box_size == 0) && (j != 0)) {
                    printf("+"); // Horizontal separator for boxes
                }
                printf("---");
            }
            printf("\n");
        }

        if (i < 10) {
            printf("%d |", i); // Row header
        } else {
            printf("%c |", 'A' + (i - 10)); // Row header for 16x16
        }

        for (int j = 0; j < grid->size; j++) {
            if (grid->board[i][j] == 0) {
                printf(" . "); // Empty cell
            } else if (grid->board[i][j] <= 9) {
                printf(" %d ", grid->board[i][j]); // Number 1-9
            } else {
                printf(" %c ", grid->board[i][j] + 'A' - 10); // Number 10-16 as A-G
            }
            if ((j + 1) % grid->box_size == 0) {
                printf("|"); // Vertical separator for boxes
            }
        }
        printf("\n");
    }
}

// Function to check if placing num in board[row][col] is valid
bool is_valid(KillerSudokuGrid* grid, int row, int col, int num) {
    for (int j = 0; j < grid->size; j++) {
        if (grid->board[row][j] == num) {
            return false;
        }
    }
    for (int i = 0; i < grid->size; i++) {
        if (grid->board[i][col] == num) {
            return false;
        }
    }
    int box_row_start = (row / grid->box_size) * grid->box_size;
    int box_col_start = (col / grid->box_size) * grid->box_size;
    for (int i = 0; i < grid->box_size; i++) {
        for (int j = 0; j < grid->box_size; j++) {
            if (grid->board[box_row_start + i][box_col_start + j] == num) {
                return false;
            }
        }
    }
    // Add cage validation
    for (int cage_index = 0; cage_index < grid->cage_count; cage_index++) {
        // Check if the cell is part of the cage
        for (int i = 0; i < grid->cages[cage_index].cell_count; i++) {
            if (grid->cages[cage_index].cells[i][0] == row && grid->cages[cage_index].cells[i][1] == col) {
                // Calculate current sum of the cage
                int current_sum = 0;
                for (int j = 0; j < grid->cages[cage_index].cell_count; j++) {
                    int r = grid->cages[cage_index].cells[j][0];
                    int c = grid->cages[cage_index].cells[j][1];
                    current_sum += grid->board[r][c];
                }
                // Check if adding the new number exceeds the cage sum
                if (current_sum + num > grid->cages[cage_index].sum) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Function to solve the Killer Sudoku puzzle using backtracking
bool solve_killer_sudoku(KillerSudokuGrid* grid) {
    for (int row = 0; row < grid->size; row++) {
        for (int col = 0; col < grid->size; col++) {
            if (grid->board[row][col] == 0) {
                for (int num = 1; num <= grid->size; num++) {
                    if (is_valid(grid, row, col, num)) {
                        grid->board[row][col] = num;
                        if (solve_killer_sudoku(grid)) {
                            return true;
                        }
                        grid->board[row][col] = 0; // Backtrack
                    }
                }
                return false; // No valid number found
            }
        }
    }
    return true; // Solved
}

// Function to initialize the grid based on size
bool init_grid(KillerSudokuGrid* grid, int size) {
    if (!grid || (size != 9 && size != 16)) {
        return false;
    }
    grid->size = size;
    grid->box_size = (size == 9) ? 3 : 4;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid->board[i][j] = 0; // Initialize all cells to 0
        }
    }
    grid->cage_count = 0; // Initialize cage count
    return true;
}

// Function to check if the user's input is valid
bool is_valid_input(KillerSudokuGrid* grid, char input) {
    return (input >= '0' && input <= '9') || input == '.' || (grid->size == 16 && input >= 'A' && input <= 'G');
}

// Function to generate a valid Killer Sudoku puzzle
void generate_killer_puzzle(KillerSudokuGrid* grid, KillerSudokuGrid* solution, int difficulty) {
    // First, create a solved puzzle
    init_grid(grid, grid->size);

    // Fill diagonal boxes first (they are independent)
    for (int box = 0; box < grid->size; box += grid->box_size) {
        for (int num = 1; num <= grid->size; num++) {
            int row, col;
            do {
                row = box + (rand() % grid->box_size);
                col = box + (rand() % grid->box_size);
            } while (!is_valid(grid, row, col, num));
            grid->board[row][col] = num;
        }
    }

    // Solve the rest
    solve_killer_sudoku(grid);

    // Copy the solved grid to the solution
    for (int i = 0; i < grid->size; i++) {
        for (int j = 0; j < grid->size; j++) {
            solution->board[i][j] = grid->board[i][j];
        }
    }

    // Remove numbers based on difficulty
    int cells_to_remove = 0;
    switch (difficulty) {
        case 1: // Easy
            cells_to_remove = (grid->size == 9) ? 40 : 150;
            break;
        case 2: // Medium
            cells_to_remove = (grid->size == 9) ? 50 : 180;
            break;
        case 3: // Hard
            cells_to_remove = (grid->size == 9) ? 60 : 210;
            break;
    }

    while (cells_to_remove > 0) {
        int row = rand() % grid->size;
        int col = rand() % grid->size;
        if (grid->board[row][col] != 0) {
            grid->board[row][col] = 0; // Remove the cell
            cells_to_remove--;
        }
    }
}

// Function to check if user's solution is correct
bool check_solution(KillerSudokuGrid* user_grid, KillerSudokuGrid* solution) {
    for (int i = 0; i < user_grid->size; i++) {
        for (int j = 0; j < user_grid->size; j++) {
            if (user_grid->board[i][j] != solution->board[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// Main function
int main() {
    KillerSudokuGrid grid, solution;
    int choice, size_choice, difficulty;
    char input[100];
    time_t start_time, current_time;
    int time_limit;

    srand(time(NULL));  // Initialize random seed

    while (true) { // Main loop
        printf("Welcome to Sudoku Solver!\n");
        printf("1. Solve your own puzzle\n");
        printf("2. Play generated puzzle\n");
        printf("3. Play Killer Sudoku\n"); // Option for Killer Sudoku
        printf("4. Exit\n"); // Option to exit
        printf("Enter choice (1, 2, 3, or 4): ");
        scanf("%d", &choice);
        getchar();

        if (choice == 4) {
            printf("Exiting the program...\n");
            break; // Exit the program
        }

        printf("\nChoose Sudoku size:\n");
        printf("1. 9x9\n");
        printf("2. 16x16\n");
        printf("Enter choice (1 or 2): ");
        scanf("%d", &size_choice);
        getchar();

        // Initialize grid based on choice
        if (size_choice == 1) {
            init_grid(&grid, SMALL_SIZE);
            init_grid(&solution, SMALL_SIZE);
        } else {
            init_grid(&grid, MAX_SIZE);
            init_grid(&solution, MAX_SIZE);
        }

        if (choice == 1) {
            // Manual puzzle input
            printf("\nEnter the Sudoku puzzle row by row:\n");
            printf("Use: \n");
            printf("- '.' or '0' for empty cells\n");
            printf("- 1-9 for numbers\n");
            if (grid.size == 16) {
                printf("- A-G for numbers 10-16\n");
            }

            // Input processing
            for (int i = 0; i < grid.size; i++) {
                printf("Row %d: ", i + 1);
                scanf("%s", input);

                for (int j = 0; j < grid.size; j++) {
                    if (input[j] == '.' || input[j] == '0') {
                        grid.board[i][j] = 0;
                    } else if (input[j] >= '1' && input[j] <= '9') {
                        grid.board[i][j] = input[j] - '0';
                    } else if (grid.size == 16 && input[j] >= 'A' && input[j] <= 'G') {
                        grid.board[i][j] = input[j] - 'A' + 10;
                    }
                }
                
                printf("\nCurrent Sudoku Board after entering row %d:\n", i + 1);
                print_board(&grid);
            }
            printf("\nInitial Sudoku Board:\n");
            print_board(&grid);

            if (solve_killer_sudoku(&grid)) {
                printf("\nSolved Sudoku Board:\n");
                print_board(&grid);
            } else {
                printf("\nNo solution exists for the given Sudoku puzzle.\n");
            }
        } else {
            // Generate puzzle
            printf("\nChoose difficulty level:\n");
            printf("1. Easy\n");
            printf("2. Medium\n");
            printf("3. Hard\n");
            printf("Enter choice (1-3): ");
            scanf("%d", &difficulty);
            getchar();

            printf("\nEnter time limit in seconds: ");
            scanf("%d", &time_limit);
            getchar();

            time_t start_time = time(NULL); // Start the timer

            // Generate and save solution
            generate_killer_puzzle(&grid, &solution, difficulty);
            printf("\nGenerated Sudoku Puzzle:\n");
            print_board(&grid);

            // Input processing with timer
            for (int i = 0; i < grid.size; i++) {
                printf("Row %d: ", i + 1);
                scanf("%s", input);

                // Check elapsed time
                time_t current_time = time(NULL);
                if (difftime(current_time, start_time) > time_limit) {
                    printf("\nTime's up! Here's the solution:\n");
                    print_board(&solution);
                    return 0;
                }

                for (int j = 0; j < grid.size; j++) {
                    if (input[j] == '.' || input[j] == '0') {
                        grid.board[i][j] = 0;
                    } else if (input[j] >= '1' && input[j] <= '9') {
                        grid.board[i][j] = input[j] - '0';
                    } else if (grid.size == 16 && input[j] >= 'A' && input[j] <= 'G') {
                        grid.board[i][j] = input[j] - 'A' + 10;
                    }
                }

                printf("\nCurrent Sudoku Board after entering row %d:\n", i + 1);
                print_board(&grid); // Display the current state of the grid
            }

            // Check user's solution
            if (check_solution(&grid, &solution)) {
                printf("\nCongratulations! Your solution is correct!\n");
            } else {
                printf("\nYour solution is incorrect. Here's the correct solution:\n");
                print_board(&solution);
            }
        }

        if (choice == 3) {
            // Logic for Killer Sudoku
            // Initialize grid and solution for Killer Sudoku
            init_grid(&grid, MAX_SIZE);
            init_grid(&solution, MAX_SIZE);
            // Generate or solve Killer Sudoku puzzle
            generate_killer_puzzle(&grid, &solution, difficulty);
            // Add input processing and solution checking for Killer Sudoku
        }
    }
    return 0;
}
