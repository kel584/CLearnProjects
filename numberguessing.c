#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>


int main(){
    printf("%d\n", RAND_MAX);
    srand(time(NULL));


    int guessNumber;
    int guesses = 0;
    int totalWins = 0;
    int loop = 1;
    int result;
    char answer;
    
    loop:
    int setNumber = rand() % 101;
    while(loop){

    result = scanf("%d", &guessNumber);
    if (result > 0){

    if(guessNumber > setNumber){
        printf("Lower\n");
        guesses++;
    }
    else if(guessNumber < setNumber){
        printf("Higher\n");
        guesses++;
    }
    else if(guessNumber == setNumber){
        printf("Bingo!\n");
        guesses++;
        totalWins++;
        loop = 0;   
    }
    else{
        printf("Error\n");
    }


    } else if(result <= 0){
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Invalid input.\n");
    }
    }

    printf("Do you want to continue (Y/N): ");
    scanf(" %c", &answer);
    toupper(answer);
    if(answer == 'Y'){
        loop = 1;
        printf("Results: \n");
        printf("Guesses: %d\n", guesses);
        printf("Wins: %d\n", totalWins);
        goto loop;
    } else if (answer == 'N'){
        printf("Results: \n");
        printf("Guesses: %d\n", guesses);
        printf("Wins: %d\n", totalWins);
    } else {
        printf("Error select Y or N.\n");
    }

}


