#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The link list struct has two elements at each node;
// the round winners, which is an integer array storing the winners of the round
// and the remainder represents the size of the round winners
typedef struct link_list {
    int * round_winners;
    int remainder;
    struct link_list * next;
} championship;

// This is a helper function to free the link list
void deleteTour(championship * tour){
    if (tour == NULL)
        return;
    deleteTour(tour->next);
    free(tour->round_winners);
    free(tour);
}

// In our tour championship algorithm, when the round size is odd (one element without opponent)
// we use the formula size*(divider-1)/divider to determine which element to skip. 
// If there are total of five odd rounds, at first round, we choose size*(2^5-1)/2^5 position's element to skip the round,
// at second round, we choose size*(2^4-1)/2^4, and follow this pattern 
// until last odd round with size*0/2 index elemnt to be chosen to skip
int calcualteDivider(int n){
    int result = 1;
    int num = n;
    while (num > 1){
        num = num / 2;
        if (num % 2 == 1){
            result = result * 2;
            --num;
        }
    }
    return result;
}

// This is a helper funtion to build the link list and generate the first largest element
// It keeps comparing the adjanct elements
// and after one round is over, it starts a new round and continues to compare the adjanct winners of the last round
// until only one winner remains for the round
int doTour(int divider, championship * tour){
    championship * temp = tour;
    int div = divider;
    while (temp->remainder != 1){
        if(temp->remainder % 2 == 0){
                int * winners;
                int level_size = temp->remainder / 2;
                winners = malloc(sizeof(int) * level_size);
                int count = 0;
                for (int i=0; i<temp->remainder-1; i+=2){
                    if (COMPARE(temp->round_winners[i],temp->round_winners[i+1]) == 1)
                        winners[count++] = temp->round_winners[i];
                    else
                        winners[count++] = temp->round_winners[i+1];
                }
                temp->next = calloc(1, sizeof(championship));
                temp->next->round_winners = winners;
                temp->next->remainder = count;
        }
        else{
            int * winners;
            int level_size = (temp->remainder+1) / 2;
            winners = malloc(sizeof(int) * level_size);
            int chosen = temp->remainder * (div-1) / div;
            div = div / 2;
            if (chosen % 2 == 1)
                --chosen;
            int count = 0;
            for (int i=0; i<chosen; i+=2){
                if (COMPARE(temp->round_winners[i],temp->round_winners[i+1]) == 1)
                    winners[count++] = temp->round_winners[i];
                else
                    winners[count++] = temp->round_winners[i+1];
            }
            // find the chosen element to skip
            winners[count++] = temp->round_winners[chosen];

            // continue camparing the rest of the elements
            for (int i=chosen+1; i<temp->remainder; i+=2){
                if (COMPARE(temp->round_winners[i],temp->round_winners[i+1]) == 1)
                    winners[count++] = temp->round_winners[i];
                else
                    winners[count++] = temp->round_winners[i+1];
            }
            
            temp->next = calloc(1, sizeof(championship));
            temp->next->round_winners = winners;
            temp->next->remainder = count;
        }
        temp = temp->next;
    }
    return temp->round_winners[0];
}

// This is a helper function that takes a championship tour and find the next largest element
int generateNewWinner(int divider, int last_winner, championship * tour){
    championship * temp = tour;
    int candidate = last_winner;
    int opponent, next_winner, chosen;
    int div = divider;
    int flag = 0;
    while (temp->remainder != 1){
        // If it's an odd round, find the chosen element to skip
        if (temp->remainder % 2 == 1){
            chosen = temp->remainder * (div-1) / div;
            div = div / 2;
            flag = 1;
            if (chosen % 2 == 1)
                --chosen;
        }
        if ((temp->remainder % 2 == 1) && (candidate == chosen)){
            next_winner = candidate / 2;
            temp->next->round_winners[next_winner] = temp->round_winners[candidate];
            candidate = next_winner;
            temp = temp->next;
            flag = 0;
            continue;
        }
        
        // find the opponent for the candidate
        if (flag == 1 && candidate > chosen){
            if (candidate % 2 == 0)
                opponent = candidate - 1;
            else
                opponent = candidate + 1;
            next_winner = (candidate+1) / 2;
        }
        else{
            if (candidate % 2 == 0)
                opponent = candidate + 1;
            else
                opponent = candidate - 1;
            next_winner = candidate / 2;
        }
        
        // if non of the pairs contains -1, do compare and pass the winner to the next round;
        // if one of the pairs contains -1, automatically set the other as winner to the next round;
        // if both or the pairs are -1, set -1 as the winner to the next round.
        if (temp->round_winners[candidate] != -1 && temp->round_winners[opponent] != -1){
            if (COMPARE(temp->round_winners[candidate], temp->round_winners[opponent]) == 1)
                temp->next->round_winners[next_winner] = temp->round_winners[candidate];
            else
                temp->next->round_winners[next_winner] = temp->round_winners[opponent];
        }
        else if (temp->round_winners[candidate] == -1 && temp->round_winners[opponent] != -1)
            temp->next->round_winners[next_winner] = temp->round_winners[opponent];
        else if (temp->round_winners[opponent] == -1 && temp->round_winners[candidate] != -1)
            temp->next->round_winners[next_winner] = temp->round_winners[candidate];
        else
            temp->next->round_winners[next_winner] = -1;
        candidate = next_winner;
        temp = temp->next;
        flag = 0;
    }
    return temp->round_winners[0];
}

// The main function of the doalg, it first checks if the inputs are valid.
// Then it calls doTour to build the link list and generate the first largest
// Afterward, it calls generateNewWinner k-1 times to generate the rest of the largest elements
// and copy the winner to the Best.
// Lastly it calls deleteTour to free the memory
int doalg( int n, int k, int* Best ){
    if ( n > 10000 || n < 10 || k < 1 || k > 100 || k > n){
        printf("Invald input!\n");
        return 0;
    }
    championship * tour = NULL;
    tour = calloc(1, sizeof(championship));
    tour->round_winners = malloc(sizeof(int) * n);
    for (int i=1; i<=n; i++)
        tour->round_winners[i-1] = i;
    tour->remainder = n;
    int count = 0;
    int divider = calcualteDivider(n);
    Best[count] = doTour(divider, tour);
    tour->round_winners[Best[count++]-1] = -1;
    while(count < k){
        Best[count] = generateNewWinner(divider, Best[count-1]-1, tour);
        tour->round_winners[Best[count++]-1] = -1;
    }
    deleteTour(tour);
    return 1;
}
