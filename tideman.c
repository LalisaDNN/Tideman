#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX] = {0};

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
    int strength;
} pair;

// Array of candidates
char *candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, char *name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main()
{
    printf("Enter number of candidates: ");
    scanf("%d", &candidate_count);

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    printf("Enter candidates' names: ");
    char *name;
    for (int i = 0; i < candidate_count; i++)
    {
        char name[100]; // assuming names won't exceed 99 characters
        scanf("%s", name);
        candidates[i] = malloc((strlen(name) + 1) * sizeof(char));
        strcpy(candidates[i], name);
    }

    pair_count = 0;
    int voter_count;
    printf("Number of voters: ");
    scanf("%d", &voter_count);

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int *ranks = (int *)malloc(candidate_count * sizeof(int));

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            printf("Rank %i: ", j + 1);
            char name[100];
            scanf("%s", name);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, char name[], int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int margin = preferences[i][j] - preferences[j][i];
            if (margin > 0)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pairs[pair_count].strength = margin;
                pair_count++;
            }
            if (margin < 0)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pairs[pair_count].strength = 0 - margin;
                pair_count++;
            }
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int i = 1; i < pair_count; i++)
    {
        int j = i - 1;
        pair t = pairs[i];
        for (; j >= 0 && pairs[j].strength < t.strength; j--)
        {
            pairs[j + 1] = pairs[j];
        }

        pairs[j + 1] = t;
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
bool isCycle(int winner, int loser)
{
    if (winner == loser)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i])
        {
            if (isCycle(winner, i))
            {
                return true;
            }
        }
    }

    return false;
}

void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!isCycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{

    int wins[MAX] = {0};
    int winnerIndex[MAX] = {0};
    int winnerNumber = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                break;
            }
            wins[i]++;
        }
        if (wins[i] == candidate_count)
        {
            winnerIndex[i]++;
            winnerNumber++;
        }
    }

    if (winnerNumber == 1)
    {
        printf("The winner is: ");
    }
    else
    {
        printf("%d winners: ", winnerNumber);
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (winnerIndex[i] != 0)
        {
            printf("%s; ", candidates[i]);
        }
    }

    return;
}