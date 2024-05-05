#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Define the colors

#define R "\x1B[31m"
#define G "\x1B[32m"
#define B "\x1B[34m"
#define Y "\x1B[33m"
#define P "\x1B[35m"
#define DG "\x1B[2;90m"
#define RESET "\x1B[0m"

#define ROWS 4
#define COLUMNS 5
#define DICE_NR 5
#define NUM_ROUNDS 1
#define NR_LAYOUTS 6
#define NUM_PUBLIC_GOALS 9
#define NUM_SELECTED_PUBLIC_GOALS 3
#define MAX_PUBLIC_GOALS_LENGTH 200
#define NUM_PLAYERS 2
#define DICE_BAG 90

//Define the nr of the dice of the same colour
    int ctr=0;
    int ctg=0;
    int cty=0;
    int ctb=0;
    int ctp=0;
// Define the structure for the player window
typedef struct {
    int gems[ROWS][COLUMNS];   // 2D array to store gems
    char private_goal;           // Integer for private goal
    char public_goals[NUM_SELECTED_PUBLIC_GOALS][MAX_PUBLIC_GOALS_LENGTH];  // Array to store public goals
} PlayerWindow;

typedef enum {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    NUM_COLORS // Number of colors
} Color;

typedef struct {
    int number;
    Color color;
} Die;

// Function to convert color enum value to string
const char *colorToString(Color colors) {
    switch (colors) {
        case RED:
            return "R";
        case GREEN:
            return "G";
        case BLUE:
            return "B";
        case YELLOW:
            return "Y";
        case PURPLE:
            return "P";
        default:
            return "U";
    }
}

// Function to throw five dice
void throwDice(Die *dice) {

    srand(time(NULL));

    for (int i = 0; i < DICE_NR; i++) {
        dice[i].number = (rand() % 5) + 1;
        dice[i].color = (Color)(rand() % NUM_COLORS);
 //here we check if we have more than 18 dice of a color , if so we change the dice.
        while((dice[i].color==RED && ctr==18) || (dice[i].color==GREEN && ctg==18) || (dice[i].color==BLUE && ctb==18) ||
         (dice[i].color==YELLOW && cty==18)  || (dice[i].color==PURPLE && ctp==18) )
        {
            dice[i].color =(Color)(rand() % NUM_COLORS);
        }
        if(dice[i].color==RED && ctr<18)
            ctr++;
        if(dice[i].color==GREEN && ctg<18)
            ctg++;
        if(dice[i].color==BLUE && ctb<18)
            ctb++;
        if(dice[i].color==YELLOW && cty<18)
            cty++;
        if(dice[i].color==PURPLE && ctp<18)
            ctp++;
    }
}

//Function to add die to the board
void addDie(PlayerWindow *board, int row, int col, Die die) {
    // Place the die at the chosen spot
    board->gems[row][col] = die.number;
    board->gems[row][col] += (int)die.color * 1000;
}

// Function to print the board with added dice
void printBoard(PlayerWindow *board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            int number = board->gems[i][j];
            Color color = (Color)(number / 1000);
            int value = number % 1000;
            if(value != 0){
                if(color==RED)
                    printf(R "%d:%s\t" RESET, value, colorToString(color));
                if(color==GREEN)
                    printf(G "%d:%s\t" RESET, value, colorToString(color));
                if(color==BLUE)
                    printf(B "%d:%s\t" RESET, value, colorToString(color));
                if(color==YELLOW)
                    printf(Y "%d:%s\t" RESET, value, colorToString(color));
                if(color==PURPLE)
                    printf(P "%d:%s\t" RESET, value, colorToString(color));
            }
            else
                printf("0:U\t");
        }
        printf("\n");
    }
}

// Function to sort the string ascending
void swap_strings(char *a, char *b) {
    char temp[MAX_PUBLIC_GOALS_LENGTH];
    strcpy(temp, a);
    strcpy(a, b);     
    strcpy(b, temp);  
}

// Function to read public goals from a file and select 3 randomly
void readAndSelectPublicGoals(char selectedGoals[NUM_SELECTED_PUBLIC_GOALS][MAX_PUBLIC_GOALS_LENGTH]) {
    FILE *file = fopen("public_goals.txt", "r");
    if (file == NULL) {
        perror("Error opening file.\n");
        exit(1);
    }

    char goals[NUM_PUBLIC_GOALS][MAX_PUBLIC_GOALS_LENGTH];
    int numGoals = 0;

    // Read all public goals from the file
    while (fgets(goals[numGoals], sizeof(goals[numGoals]), file) && numGoals < NUM_PUBLIC_GOALS) {
        numGoals++;
    }

    fclose(file);

    // Select 3 random public goals
    do{
        for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
            int randomIndex = rand() % numGoals;
            strcpy(selectedGoals[i], goals[randomIndex]);
        
        }
    }while(strcmp(selectedGoals[0], selectedGoals[1]) == 0 || strcmp(selectedGoals[0], selectedGoals[2]) == 0 || strcmp(selectedGoals[1], selectedGoals[2]) == 0);

     // Sort the selected goals
    char str1[MAX_PUBLIC_GOALS_LENGTH];
    char str2[MAX_PUBLIC_GOALS_LENGTH];
    char str3[MAX_PUBLIC_GOALS_LENGTH];
    
    strcpy(str1, selectedGoals[0]);
    strcpy(str2, selectedGoals[1]);
    strcpy(str3, selectedGoals[2]);

    // Compare the first characters of the strings and swap if necessary
    if (strncmp(str1, str2, 1) > 0) {
        swap_strings(str1, str2);
    }
    if (strncmp(str1, str3, 1) > 0) {
        swap_strings(str1, str3);
    }
    if (strncmp(str2, str3, 1) > 0) {
        swap_strings(str2, str3);
    }

    // Copy the sorted strings back into the selectedGoals array
    strcpy(selectedGoals[0], str1);
    strcpy(selectedGoals[1], str2);
    strcpy(selectedGoals[2], str3);

}

void readLayoutsFromFile(char layouts[][ROWS][COLUMNS][4], int numLayouts){
    FILE *file=fopen("player_window_layout.txt", "r");
    if(file==NULL){
        perror("Couldn't open file\n");
        exit(1);
    }
    for(int p=0; p<numLayouts; p++){
        for(int i=0;i<ROWS;i++){
            for(int j=0;j<COLUMNS;j++){
                fscanf(file,"%3s",layouts[p][i][j]);
            }
        }
    }
    fclose(file);
}

void printLayout(char layout[ROWS][COLUMNS][4]){
    for(int i=0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            int if_color_exists=0;
            if(strchr(layout[i][j],'R')!=0)
                {
                    printf(R "%-4s" RESET,layout[i][j]);
                    if_color_exists=1;
                }
            if(strchr(layout[i][j],'G')!=0)
                {
                    printf(G "%-4s" RESET,layout[i][j]);
                    if_color_exists=1;
                }
            if(strchr(layout[i][j],'Y')!=0)
                {
                    printf(Y "%-4s" RESET,layout[i][j]);
                    if_color_exists=1;
                }
            if(strchr(layout[i][j],'B')!=0)
                {
                    printf(B "%-4s" RESET,layout[i][j]);
                    if_color_exists=1;
                }
            if(strchr(layout[i][j],'P')!=0)
                {
                    printf(P "%-4s" RESET,layout[i][j]);
                    if_color_exists=1;
                }
            if(if_color_exists==0)
                printf("%-4s",layout[i][j]);
        }
        printf("\n");
    }
}
void selectRandomLayouts(char selectedLayouts[][ROWS][COLUMNS][4],char layouts[][ROWS][COLUMNS][4]){
    srand(time(NULL));
    int randomindex1=rand() % NR_LAYOUTS;
    int randomindex2;
    do{
        randomindex2=rand() % NR_LAYOUTS;
    }while(randomindex1==randomindex2);
    memcpy(selectedLayouts[0],layouts[randomindex1],sizeof(selectedLayouts[0]));
    memcpy(selectedLayouts[1],layouts[randomindex2],sizeof(selectedLayouts[1]));
}

// Function to check if the given row and column are adjacent to any previously placed die
bool isAdjacent(PlayerWindow *player, int row, int col) {
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < ROWS && j >= 0 && j < COLUMNS && player->gems[i][j] != 0) {
                return true;
            }
        }
    }
    return false;
}

// Function to check if the given row and column are adjacent to a die of the same color or value
bool isAdjacentToSame(PlayerWindow *player, int row, int col, Die die) {
    // Check adjacent cells on the same row
    for (int i = col - 1; i <= col + 1; i++) {
        if (i >= 0 && i < COLUMNS && player->gems[row][i] != 0) {
            int number = player->gems[row][i] % 1000;
            Color color = (Color)(player->gems[row][i] / 1000);
            // Check if adjacent cell has the same color or value
            if (color == die.color || number == die.number) {
                return true;
            }
        }
    }
    // Check adjacent cells on the same column
    for (int j = row - 1; j <= row + 1; j++) {
        if (j >= 0 && j < ROWS && player->gems[j][col] != 0) {
            int number = player->gems[j][col] % 1000;
            Color color = (Color)(player->gems[j][col] / 1000);
            // Check if adjacent cell has the same color or value
            if (color == die.color || number == die.number) {
                return true;
            }
        }
    }
    return false;
}

// Function to validate if the given row and column are valid for the current turn
bool isValidAdjacent(PlayerWindow *player, int row, int col, Die die) {
    // Check if the position is within the board bounds
    if (row < 0 || row >= ROWS || col < 0 || col >= COLUMNS) {
        return false;
    }
    // Check if the position is adjacent to any previously placed die
    if (!isAdjacent(player, row, col)) {
        return false;
    }
     // Check if the position is adjacent to a die of the same color or value
    if (isAdjacentToSame(player, row, col, die)) {
        return false;
    }
    //Return true if it passes every conidition
    return true;
}

// Define the player turn function
void playerTurn(PlayerWindow *player, char privateGoal, char layout[ROWS][COLUMNS][4], Die dice[DICE_NR], int availableDice[DICE_NR], int playernr, int FirstDie) {
    int dieNum, row, col;
    // Print player's private goal and layout
    printf(DG "Player %d's private goal: " RESET,playernr);
    switch (privateGoal) {
        case 0:
            printf(R "Red. Sum of values on red dice.\n" RESET);
            break;
        case 1:
            printf(G "Green. Sum of values on green dice.\n" RESET);
            break;
        case 2:
            printf(B "Blue. Sum of values on blue dice.\n" RESET);
            break;
        case 3:
            printf(Y "Yellow. Sum of values on yellow.\n" RESET);
            break;
        case 4:
            printf(P "Purple. Sum of values on purple dice.\n" RESET);
            break;
    }
    printf(DG "\nPlayer %d's Layout: \n\n" RESET,playernr);
    printLayout(layout);
    printf("\n");
    printf(DG "Player %d's current board: \n\n" RESET,playernr);
    printBoard(player);
    printf("\n");

    // Print the available dice
    printf(DG "The dice are: \n" RESET);
    for (int i = 0; i < DICE_NR; i++) {
        switch (dice[i].color) {
            case RED:
                printf(R "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
                break;
            case GREEN:
                printf(G "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
                break;
            case BLUE:
                printf(B "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
                break;
            case YELLOW:
                printf(Y "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
                break;
            case PURPLE:
                printf(P "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
                break;
            default:
                break;
        }
    }
    printf("\n");
    printf(R "Available dice are: " RESET);
    for (int i = 0; i < DICE_NR; i++) {
        if (availableDice[i]) {
            printf("%d ", i + 1);
        }
    }
    printf("\n");

    // Get input for placing the die
    bool validInput = false;

    while (!validInput) {
        printf(DG "Enter die number, row, and column for the chosen die(or enter 0 to skip turn): "RESET);
        scanf("%d", &dieNum);

        if(dieNum == 0){
            if(FirstDie < 2){
                printf(R "In the first round you have to place a die.\n" RESET);
                sleep(1);
                continue;
            } else {
                printf("Player %d chooses to skip turn.\n",playernr);
                printf("\n---------------------------------------------------------------\n\n");
                return;
            }
        }
        
        scanf("%d %d", &row, &col);

        if (dieNum<0 || dieNum>DICE_NR || row < 1 || row > ROWS || col < 1 || col > COLUMNS) {
            printf(R "Invalid input. Please enter a valid die number, row(1-4), and column(1-5).\n" RESET);
            sleep(1);
            continue;
        }
        //Then we set row and column in 0 basis indexing
        row--;
        col--;
        
        //Compare the position you entered to the layout conditions and check if it matches it
        bool validPosition = false;

        if (strcmp(layout[row][col], "0:U") == 0) {
            validPosition = true;
        } else {
            for (int i = 0; i < strlen(layout[row][col]); i++) {
                if (layout[row][col][i] == colorToString(dice[dieNum - 1].color)[0] ||
                    (layout[row][col][i] == 'U' && (dice[dieNum - 1].number == atoi(&layout[row][col][0])))) {
                    validPosition = true;
                    break;
                }
            }
        }


        if (validPosition) {
            if (player->gems[row][col] != 0) {
                printf(R "Cannot place die at row %d, column %d. Location already occupied.\n" RESET, row+1, col+1);
                sleep(1);
                continue;
            }
        }

        if (!isValidAdjacent(player, row, col, dice[dieNum-1]) && FirstDie>1) {
            printf(R "Invalid placement. The die must be adjacent to a previously placed die and not adjacent to a die of the same color or value.\n" RESET);
            sleep(1);
            continue;
        }
        if(validPosition){
            if (FirstDie == 0 || FirstDie == 1) {
                if (dieNum >= 1 && dieNum <= DICE_NR && availableDice[dieNum - 1] != 0 && ((row == 0 || row == ROWS - 1) || (col == 0 || col == COLUMNS - 1))) {
                    validInput = true;
                    addDie(player, row, col, dice[dieNum - 1]);
                    availableDice[dieNum - 1] = 0;
                } else {
                    printf(R "The first die should be placed on an edge or in a corner, also don't forget to choose an available die.\n" RESET);
                    sleep(1);
                }
            } else {
                if (dieNum >= 1 && dieNum <= DICE_NR && availableDice[dieNum - 1] != 0 && row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
                    validInput = true;
                    addDie(player, row, col, dice[dieNum - 1]);
                    availableDice[dieNum - 1] = 0;
                } else {
                    printf(R "Invalid input. Please enter a valid die number, row (1-4), and column (1-5).\n" RESET);
                    sleep(1);
                }
            }
            } else {
                printf(R "Invalid position. Please choose a position that matches the layout.\n" RESET);
                sleep(1);
            }
    }

    // Print the updated player's board
    printf(DG "\nPlayer %d's Board:\n"RESET, playernr);
    printBoard(player);
    printf("\n---------------------------------------------------------------\n\n");
}

// Function to calculate private goal score
int scorePrivateGoal(PlayerWindow *player) {
    int score = 0;

    // Iterate through the player's board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            int number = player->gems[i][j] % 1000;
            Color color = (Color)(player->gems[i][j] / 1000);

            // Check if the color of the die matches the private goal color
            if (color == player->private_goal) {
                // Add the value of the die to the score
                score += number;
            }
        }
    }

    return score;
}

// Function to calculate score for "Column Color Variety" public goal
int scoreColumnColorVariety(PlayerWindow *player) {
    int score = 0;
    // Iterate through each column
    for (int j = 0; j < COLUMNS; j++) {
        bool hasRepeatedColor = false;
        Color firstColor = NUM_COLORS; // Initialize with an invalid color
        Color secondColor = NUM_COLORS; // Initialize with an invalid color
        Color thirdColor = NUM_COLORS; // Initialize with an invalid color
        Color fourthColor = NUM_COLORS; // Initialize with an invalid color
        // Iterate through each row in the current column
        for (int i = 0; i < ROWS; i++) {
            //Check if it blank
            if(player->gems[i][j]==0){
                hasRepeatedColor=true;
                break;
            }
            // Extract the color of the die
            Color color = (Color)(player->gems[i][j] / 1000);
            //Store every color from the row
            switch(i){
                case 0:
                    firstColor = color;
                    break;
                case 1:
                    secondColor = color;
                    break;
                case 2:
                    thirdColor = color;
                    break;
                case 3:
                    fourthColor = color;
                    break;
                default:
                    break;
            }
        }

        if(hasRepeatedColor) continue;

        if(firstColor == secondColor || firstColor == thirdColor || firstColor == fourthColor || secondColor == thirdColor || 
        secondColor == fourthColor || thirdColor == fourthColor){
            hasRepeatedColor=true;
        }
        // If no repeated colors found in the column, add points to the score
        if (!hasRepeatedColor) {
            score += 5;
        }
    }
    return score;
}

// Function to calculate score for "Row Color Variety" public goal
int scoreRowColorVariety(PlayerWindow *player) {
    int score = 0;
    // Iterate through each column
    for (int i = 0; i < ROWS; i++) {
        bool hasRepeatedColor = false;
        Color firstColor = NUM_COLORS; // Initialize with an invalid color
        Color secondColor = NUM_COLORS; // Initialize with an invalid color
        Color thirdColor = NUM_COLORS; // Initialize with an invalid color
        Color fourthColor = NUM_COLORS; // Initialize with an invalid color
        Color fifthColor = NUM_COLORS; // Initialize with an invalid color
        // Iterate through each row in the current column
        for (int j = 0; j < COLUMNS; j++) {
            //Check if it blank
            if(player->gems[i][j]==0){
                hasRepeatedColor=true;
                break;
            }
            // Extract the color of the die
            Color color = (Color)(player->gems[i][j] / 1000);
            //Store every color from the row
            switch(j) {
                case 0:
                    firstColor = color;
                    break;
                case 1:
                    secondColor = color;
                    break;
                case 2:
                    thirdColor = color;
                    break;
                case 3:
                    fourthColor = color;
                    break;
                case 4:
                    fifthColor = color;
                    break;
                default:
                    break;
            }
        }
    
        if(hasRepeatedColor) continue;

        if(firstColor == secondColor || firstColor == thirdColor || firstColor == fourthColor || firstColor == fifthColor 
        || secondColor == thirdColor || secondColor == fourthColor|| secondColor == fifthColor || thirdColor == fourthColor || 
        thirdColor == fifthColor || fourthColor == fifthColor){
            hasRepeatedColor=true;
        }
        // If no repeated colors found in the column, add points to the score
        if (!hasRepeatedColor) {
            score += 6;
        }
    }
    return score;
}

//Function to calculate score for "Color Variety" public goal
int scoreColorVariety(PlayerWindow *player) {
    int red=0;
    int green=0;
    int blue=0;
    int yellow=0;
    int purple=0;
    int min=0;

    // Iterate through the player's board to calculte how many dice of each color are there
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if(player->gems[i][j] == 0)
                continue;
            Color color = (Color)(player->gems[i][j] / 1000);
            if(color == RED){
                red++;
            }
            if(color == GREEN){
                green++;
            }           
            if(color == BLUE){
                blue++;
            }
            if(color == YELLOW){
                yellow++;
            }
            if(color == PURPLE){
                purple++;
            } 
        }
    }
    //To check Color Variety we calculate the minimum of each color dice placed, beacuse that is the lowest nr of sets that include all colors
    min=red;
    if(green<min) min=green;
    if(blue<min) min=blue;
    if(yellow<min) min=yellow;
    if(purple<min) min=purple;

    min=min*4;
    return min;
}

//Function to calculate "Light Shades" public goal
int scoreLightShades(PlayerWindow *player) {
    int nr1=0;
    int nr2=0;
    int min=0;
    // Iterate through the player's board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if(player->gems[i][j] == 0)
                continue;
            int number = player->gems[i][j] % 1000;
            if(number == 1) nr1++;
            if(number == 2) nr2++;
        }
    }

    min = nr1;
    if(nr2 < min) min = nr2;
    min=min*2;
    return min;
}

//Function to calculate "Medium Shades" public goal
int scoreMediumShades(PlayerWindow *player) {
    int nr3=0;
    int nr4=0;
    int min=0;
    // Iterate through the player's board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if(player->gems[i][j] == 0)
                continue;
            int number = player->gems[i][j] % 1000;
            if(number == 3) nr3++;
            if(number == 4) nr4++;
        }
    }

    min = nr3;
    if(nr4 < min) min = nr4;
    min=min*2;
    return min;
}

//Function to calculate "Deep Shades" public goal
int scoreDeepShades(PlayerWindow *player) {
    int nr5=0;
    int nr6=0;
    int min=0;
    // Iterate through the player's board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if(player->gems[i][j] == 0)
                continue;
            int number = player->gems[i][j] % 1000;
            if(number == 5) nr5++;
            if(number == 6) nr6++;
        }
    }

    min = nr5;
    if(nr6 < min) min = nr6;
    min=min*2;
    return min;
}

// Function to calculate score for "Column Shade Variety" public goal
int scoreColumnShadeVariety(PlayerWindow *player){
    int score=0;

    for(int j=0; j < COLUMNS; j++){
        bool hasRepeteadNumber = false;
        int firstNumber=0;
        int secondNumber=0;
        int thirdNumber=0;
        int fourthNumber=0;
        // Iterate through each row in the current column
        for(int i=0; i<ROWS;i++){
            if(player->gems[i][j]==0){
                hasRepeteadNumber=true;
                break;
            }
            int number=player->gems[i][j]%1000;
            switch(i){
                case 0:
                    firstNumber=number;
                    break;
                case 1:
                    secondNumber=number;
                    break;
                case 2:
                    thirdNumber=number;
                    break;
                case 3:
                    fourthNumber=number;
                    break;
                default:
                    break;
            }
        }

        if(hasRepeteadNumber) continue;

        if(firstNumber == secondNumber || firstNumber == thirdNumber || firstNumber == fourthNumber || secondNumber == thirdNumber || 
        secondNumber == fourthNumber || thirdNumber == fourthNumber){
            hasRepeteadNumber=true;
        }
        if(!hasRepeteadNumber)
        score+=4;
    }

    return score;
}

// Function to calculate score for "Row Shade Variety" public goal
int scoreRowShadeVariety(PlayerWindow *player){
    int score=0;

    for(int i=0; i < ROWS; i++){
        bool hasRepeteadNumber = false;
        int firstNumber=0;
        int secondNumber=0;
        int thirdNumber=0;
        int fourthNumber=0;
        int fifthNumber=0;
        // Iterate through each row in the current column
        for(int j=0; j < COLUMNS; j++){
            if(player->gems[i][j]==0){
                hasRepeteadNumber=true;
                break;
            }
            int number=player->gems[i][j]%1000;
            switch(j){
                case 0:
                    firstNumber=number;
                    break;
                case 1:
                    secondNumber=number;
                    break;
                case 2:
                    thirdNumber=number;
                    break;
                case 3:
                    fourthNumber=number;
                    break;
                case 4:
                    fifthNumber=number;
                    break;
                default:
                    break;
            }
        }

        if(hasRepeteadNumber) continue;

        if(firstNumber == secondNumber || firstNumber == thirdNumber || firstNumber == fourthNumber || firstNumber == fifthNumber ||
        secondNumber == thirdNumber || secondNumber == fourthNumber || secondNumber == fifthNumber ||thirdNumber == fourthNumber || 
        thirdNumber == fifthNumber || fourthNumber == fifthNumber){
            hasRepeteadNumber=true;
        }
        if(!hasRepeteadNumber)
        score+=5;
    }

    return score;
}

//Function to calculate score for "Shade Variety" public goal
int scoreShadeVariety(PlayerWindow *player) {
    int nr1=0,nr2=0,nr3=0,nr4=0,nr5=0,nr6=0;
    int min=0;
    // Iterate through the player's board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if(player->gems[i][j] == 0)
                continue;
            int number = player->gems[i][j] % 1000;
            if(number == 1) nr1++;
            if(number == 2) nr2++;
            if(number == 3) nr3++;
            if(number == 4) nr4++;
            if(number == 5) nr5++;
            if(number == 6) nr6++;
        }
    }

    min = nr1;
    if(nr2 < min) min = nr2;
    if(nr3 < min) min = nr3;
    if(nr4 < min) min = nr4;
    if(nr5 < min) min=nr5;
    if(nr6 < min) min = nr6;
    min=min*5;
    return min;
}

void startgame(){
    char layouts[NR_LAYOUTS][ROWS][COLUMNS][4];
    char selectedLayouts[NUM_PLAYERS][ROWS][COLUMNS][4];
    readLayoutsFromFile(layouts,NR_LAYOUTS);
    selectRandomLayouts(selectedLayouts,layouts);
    int firstdie=0;
    // Create instances of the PlayerWindow structure for both players
    PlayerWindow player1, player2;
    srand(time(NULL));

    // Initialize gems array for both players
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            player1.gems[i][j] = 0;  
            player2.gems[i][j] = 0;  
        }
    }

    // Set private goals for both players
    player1.private_goal = rand() % 5;
    do{
    player2.private_goal = rand() % 5;
    }while(player1.private_goal == player2.private_goal);

    // Set public goals for both players
    char selectedGoals[NUM_SELECTED_PUBLIC_GOALS][MAX_PUBLIC_GOALS_LENGTH];
    readAndSelectPublicGoals(selectedGoals);

    // Assign public goals to both players
    for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
        strcpy(player1.public_goals[i], selectedGoals[i]);
        strcpy(player2.public_goals[i], selectedGoals[i]);
    }

    // Print information for both players
    printf(DG "Player 1 Window:\n" RESET);
    printf(DG "Layout for Player 1 is: \n\n" RESET);
    printLayout(selectedLayouts[0]);
    printf("\n");
    // Print the public goals array
    printf(DG "Public Goals:\n" RESET);
    //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
    for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
        // Replace newline characters with spaces
        for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
            if (player1.public_goals[i][j] == '\n') {
                player1.public_goals[i][j] = ' ';
            }
        }
        printf("%s\n", player1.public_goals[i]);
    }
    printf("\n");
    sleep(4);

    printf(DG "Player 2 Window:\n" RESET);
    printf(DG "Layout for Player 2 is: \n\n" RESET);
    printLayout(selectedLayouts[1]);
    printf("\n");
    printf(DG "Public Goals:\n" RESET);
    //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
    for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
        // Replace newline characters with spaces
        for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
            if (player2.public_goals[i][j] == '\n') {
                player2.public_goals[i][j] = ' ';
            }
        }
        printf("%s\n", player2.public_goals[i]);
    }
    printf("\n");
    sleep(4);

    printf(R "START GAME. Enjoy!\n\n" RESET);
    sleep(2);

    // Loop for multiple rounds
    for (int round = 1; round <= NUM_ROUNDS; round++) {
        int availableDice[DICE_NR]={1,2,3,4,5};
        printf("---------------------------------------------------------------\n");
        printf(G "Round %d\n"RESET, round);

        // Throw five dice for each round
        Die dice[DICE_NR];
        throwDice(dice);

        // Print the results for the current round
        printf(DG "\nResults of the dice:\n" RESET);
        for (int i = 0; i < DICE_NR; i++) {
            if(dice[i].color==RED)
                printf(R "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
            if(dice[i].color==GREEN)
                printf(G "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
            if(dice[i].color==BLUE)
                printf(B "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
            if(dice[i].color==YELLOW)
                printf(Y "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));
            if(dice[i].color==PURPLE)
                printf(P "Die %d: Number: %d, Color: %s\n" RESET, i + 1, dice[i].number, colorToString(dice[i].color));

        }

        printf("\n---------------------------------------------------------------\n");
       sleep(4);

        printf(R "Player 1's turn: \n" RESET);
        printf(G "\nEnter the die number (1-5), row, and column for each die (1-4 for row, 1-5 for column):\n\n" RESET);
        printf(DG "Public Goals:\n" RESET);
        //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
        for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
            // Replace newline characters with spaces
            for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
                if (player1.public_goals[i][j] == '\n') {
                    player1.public_goals[i][j] = ' ';
                }
            }
            printf("%s\n", player1.public_goals[i]);
        }
        printf("\n");
        playerTurn(&player1, player1.private_goal, selectedLayouts[0], dice, availableDice,1,firstdie);
        firstdie++;
        sleep(3);

        printf(R "Player 2's turn: \n" RESET);
        printf(G "\nEnter the die number (1-5), row, and column for each die (1-4 for row, 1-5 for column):\n\n" RESET);
        printf(DG "Public Goals:\n" RESET);
        //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually. 
        for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
            // Replace newline characters with spaces
            for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
                if (player2.public_goals[i][j] == '\n') {
                    player2.public_goals[i][j] = ' ';
                }
            }
            printf("%s\n", player2.public_goals[i]);
        }
        printf("\n");
        playerTurn(&player2, player2.private_goal, selectedLayouts[1], dice, availableDice,2,firstdie);
        firstdie++;
        sleep(3);

        printf(R "Player 2's turn: \n" RESET);
        printf(G "\nEnter the die number (1-5), row, and column for each die (1-4 for row, 1-5 for column):\n\n" RESET);
        printf(DG "Public Goals:\n" RESET);
        //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
        for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
            // Replace newline characters with spaces
            for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
                if (player2.public_goals[i][j] == '\n') {
                    player2.public_goals[i][j] = ' ';
                }
            }
            printf("%s\n", player2.public_goals[i]);
        }
        printf("\n");
        playerTurn(&player2, player2.private_goal, selectedLayouts[1], dice, availableDice,2,firstdie);
        sleep(3);

        printf(R "Player 1's turn: \n" RESET);
        printf(G "\nEnter the die number (1-5), row, and column for each die (1-4 for row, 1-5 for column):\n\n" RESET);
        printf(DG "Public Goals:\n" RESET);
        //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
        for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
            // Replace newline characters with spaces
            for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
                if (player1.public_goals[i][j] == '\n') {
                    player1.public_goals[i][j] = ' ';
                }
            }
            printf("%s\n", player1.public_goals[i]);
        }
        printf("\n");
        playerTurn(&player1, player1.private_goal, selectedLayouts[0], dice, availableDice,1,firstdie);
        sleep(3);
    }
    printf(R "END OF GAME!" RESET);
    printf("\n\n---------------------------------------------------------------\n\n");
    int PrivateGoalScore1=scorePrivateGoal(&player1);
    int PrivateGoalScore2=scorePrivateGoal(&player2);
    sleep(2);
     printf(DG "Player 1 private goal: " RESET);
    switch (player1.private_goal) {
        case 0:
            printf(R "Red. Sum of values on red dice.\n" RESET);
            break;
        case 1:
            printf(G "Green. Sum of values on green dice.\n" RESET);
            break;
        case 2:
            printf(B "Blue. Sum of values on blue dice.\n" RESET);
            break;
        case 3:
            printf(Y "Yellow. Sum of values on yellow.\n" RESET);
            break;
        case 4:
            printf(P "Purple. Sum of values on purple dice.\n" RESET);
            break;
    }
    printf(DG "Player 1's Board: \n" RESET);
    printBoard(&player1);
    printf("\n");
    sleep(2);

     printf(DG "Player 2 private goal: " RESET);
    switch (player2.private_goal) {
        case 0:
            printf(R "Red. Sum of values on red dice.\n" RESET);
            break;
        case 1:
            printf(G "Green. Sum of values on green dice.\n" RESET);
            break;
        case 2:
            printf(B "Blue. Sum of values on blue dice.\n" RESET);
            break;
        case 3:
            printf(Y "Yellow. Sum of values on yellow.\n" RESET);
            break;
        case 4:
            printf(P "Purple. Sum of values on purple dice.\n" RESET);
            break;
    }
    printf(DG "Player 2's Board: \n" RESET);
    printBoard(&player2);
    printf("\n");
    sleep(2);



    printf(DG "Private Goal Score for Player 1:" RESET " %d\n", PrivateGoalScore1);
    printf(DG "Private Goal Score for Player 2:" RESET " %d\n\n", PrivateGoalScore2);
    sleep(2);

    // Print the public goals array
    printf(DG "Public Goals:\n" RESET);
    //Because sometimes printf didn't handle the end of the line properly, so i've put insetead of \n in the array a space and put \n manually.
    for (int i = 0; i < NUM_SELECTED_PUBLIC_GOALS; i++) {
        // Replace newline characters with spaces
        for (int j = 0; player1.public_goals[i][j] != '\0'; j++) {
            if (player1.public_goals[i][j] == '\n') {
                player1.public_goals[i][j] = ' ';
            }
        }
        printf("%s\n", player1.public_goals[i]);
    }
    printf("\n");
    sleep(2);
    int TotalScore1=PrivateGoalScore1;
    int TotalScore2=PrivateGoalScore2;

    bool has_column_variety=false;
    bool has_row_variety=false;
    bool has_color_variety=false;
    bool has_light_shades=false;
    bool has_medium_shades=false;
    bool has_deep_shades=false;
    bool has_column_shade_variety=false;
    bool has_row_shade_variety=false;
    bool has_shade_variety=false;

    for(int i=0; i<NUM_SELECTED_PUBLIC_GOALS;i++){
        if(strncmp(selectedGoals[i],"1)",1) == 0) has_column_variety=true;
        if(strncmp(selectedGoals[i],"2)",1) == 0) has_row_variety=true;
        if(strncmp(selectedGoals[i],"3)",1) == 0) has_color_variety=true;
        if(strncmp(selectedGoals[i],"4)",1) == 0) has_light_shades=true;
        if(strncmp(selectedGoals[i],"5)",1) == 0) has_medium_shades=true;
        if(strncmp(selectedGoals[i],"6)",1) == 0) has_deep_shades=true;
        if(strncmp(selectedGoals[i],"7)",1) == 0) has_column_shade_variety=true;
        if(strncmp(selectedGoals[i],"8)",1) == 0) has_row_shade_variety=true;
        if(strncmp(selectedGoals[i],"9)",1) == 0) has_shade_variety=true;
    }

    if(has_column_variety){
        int ColumnVarietyScore1=scoreColumnColorVariety(&player1);
        int ColumnVarietyScore2=scoreColumnColorVariety(&player2);
        printf(G "Score from Column Color Variety:\n" RESET );
        printf(DG "For Player 1:" RESET " %d\n",ColumnVarietyScore1);
        printf(DG "For Player 2:" RESET" %d\n\n",ColumnVarietyScore2);
        TotalScore1+=ColumnVarietyScore1;
        TotalScore2+=ColumnVarietyScore2;    
        sleep(2);    
    }
    if(has_row_variety){
        int RowVarietyScore1=scoreRowColorVariety(&player1);
        int RowVarietyScore2=scoreRowColorVariety(&player2);
        printf(G "Score from Row Color Variety:\n" RESET);
        printf(DG "For Player 1:" RESET " %d\n",RowVarietyScore1);
        printf(DG "For Player 2:" RESET " %d\n\n",RowVarietyScore2);
        TotalScore1+=RowVarietyScore1;
        TotalScore2+=RowVarietyScore2;
        sleep(2);
    }
    if(has_color_variety){
        int ColorVarietyScore1=scoreColorVariety(&player1);
        int ColorVarietyScore2=scoreColorVariety(&player2);
        printf(G "Score from Color Variety:\n" RESET);
        printf(DG "For Player 1:"RESET" %d\n",ColorVarietyScore1);
        printf(DG "For Player 2:"RESET" %d\n\n",ColorVarietyScore2);
        TotalScore1+=ColorVarietyScore1;
        TotalScore2+=ColorVarietyScore2;
        sleep(2);
    }
    if(has_light_shades){
        int LightShadesScore1=scoreLightShades(&player1);
        int LightShadesScore2=scoreLightShades(&player2);
        printf(G"Score from Light Shades:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",LightShadesScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",LightShadesScore2);
        TotalScore1+=LightShadesScore1;
        TotalScore2+=LightShadesScore2;
        sleep(2);
    }
    if(has_medium_shades){
        int MediumShadesScore1=scoreMediumShades(&player1);
        int MediumShadesScore2=scoreMediumShades(&player2);
        printf(G"Score from Medium Shades:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",MediumShadesScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",MediumShadesScore2);
        TotalScore1+=MediumShadesScore1;
        TotalScore2+=MediumShadesScore2;
        sleep(2);
    }
    if(has_deep_shades){
        int DeepShadesScore1=scoreDeepShades(&player1);
        int DeepShadesScore2=scoreDeepShades(&player2);
        printf(G"Score from Deep Shades:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",DeepShadesScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",DeepShadesScore2);
        TotalScore1+=DeepShadesScore1;
        TotalScore2+=DeepShadesScore2;
        sleep(2);
    }
    if(has_column_shade_variety){
        int ColumnShadeVarietyScore1=scoreColumnShadeVariety(&player1);
        int ColumnShadeVarietyScore2=scoreColumnShadeVariety(&player2);
        printf(G"Score from Column Shade Variety:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",ColumnShadeVarietyScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",ColumnShadeVarietyScore2);
        TotalScore1+=ColumnShadeVarietyScore1;
        TotalScore2+=ColumnShadeVarietyScore2;
        sleep(2);
    }
    if(has_row_shade_variety){
        int RowShadeVarietyScore1=scoreRowShadeVariety(&player1);
        int RowShadeVarietyScore2=scoreRowShadeVariety(&player2);
        printf(G"Score from Row Shade Variety:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",RowShadeVarietyScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",RowShadeVarietyScore2);
        TotalScore1+=RowShadeVarietyScore1;
        TotalScore2+=RowShadeVarietyScore2;
        sleep(2);
    }
    if(has_shade_variety){
        int ShadeVarietyScore1=scoreShadeVariety(&player1);
        int ShadeVarietyScore2=scoreShadeVariety(&player2);
        printf(G"Score from Shade Variety:\n"RESET);
        printf(DG"For Player 1:"RESET" %d\n",ShadeVarietyScore1);
        printf(DG"For Player 2:"RESET" %d\n\n",ShadeVarietyScore2);
        TotalScore1+=ShadeVarietyScore1;
        TotalScore2+=ShadeVarietyScore2;
        sleep(2);
    }   
    printf(B"TOTAL SCORE FOR PLAYER 1 IS: " RESET Y"%d\n"RESET,TotalScore1);
    sleep(3);
    printf(B"TOTAL SCORE FOR PLAYER 2 IS: " RESET Y"%d\n\n"RESET,TotalScore2);
    sleep(2);
    if(TotalScore1>TotalScore2){
        printf(R "Player 1 Wins!\n" RESET);
    }else if(TotalScore2>TotalScore1){
        printf(R "Player 2 Wins!\n" RESET);
    }
    else{
        printf(R "It's a tie!\n"RESET);
    }
}
int main() {
    startgame();
    return 0;
}
//This is the last version