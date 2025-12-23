#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
bool ccycle(int wi, int lo); //--> custom function, recursion to explore paths 
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
        printf("Recorded Candidates are: Candidate[%i] = %s\n", i, candidates[i]);
     }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            {
                locked[i][j] = false;
            }
        }
    }
    
    pair_count = 0;
    int voter_count = get_int("Number of voters: ");
     
    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's i-th preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }
        record_preferences(ranks);
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
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
    //int i;
    //int j;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
           if (preferences[ranks[i]][ranks[j]] != 0)
           {
                preferences[ranks[i]][ranks[j]]++;
           }
           else 
           {
                preferences[ranks[i]][ranks[j]]++;
           }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Implementing the main 2d loop on preferences[i(MAX)][j(MAX)] 
    // Implementig a pair index to loop through the pairs array index

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            // Implementing the 3 cases of exclusion:
            // a. Identity, i == j, p[s][s]
            // b. No vote received, p[i][j] == 0
            // c. No winner in a given pair, ex. p[0][1] == p[1][0] == n                   // *** new condition to be checked *** //
            if (i == j /* identity */ || preferences[i][j] == preferences[j][i] /* tie */ || preferences[i][j] < preferences[j][i]) 
            {
                continue;
            }
            else 
            {
            // Add the relevant pairs to the pairs[] array
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                ++pair_count;  
            }
        }
        printf("\n");
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Selection Sort Style: optimized Version with SINGLE SWAP
    for (int i = 0; i < pair_count; i++)
    {   
        int i_max = i;

        for (int j = i + 1; j < pair_count; j++)
        {
            if (preferences[pairs[j].winner][pairs[j].loser] > preferences[pairs[i_max].winner][pairs[i_max].loser])
            {   
                i_max = j;
            }
        }   
        // Swap. Swapping should happen at the end of each j-cycle of iterations.
        if (i_max != i)
        {
            int temp_i_winner = pairs[i].winner;
            int temp_i_loser = pairs[i].loser;

            pairs[i].winner = pairs[i_max].winner;
            pairs[i].loser = pairs[i_max].loser;
              
            pairs[i_max].winner = temp_i_winner;
            pairs[i_max].loser = temp_i_loser;
        }  
    }
    return;
}

// Custom and additional Recursive Helper Function for lock_pairs
bool ccycle(int wi, int lo)
{
    //bool state; 
    for (int j = 0; j < candidate_count; j++)
    {
        // check for next p\ossible connection
        if (locked[lo][j] == 1) // -> I have a connection
        {
            if (j == wi)
            {
                return false; 
            }   
            else
            {
                printf("launching recursion\n");
                    if (!ccycle(wi, j))
                        return false;
            }
        }
        else
        {
            if (j == candidate_count - 1)
                return true;
        }    
    }
    return true;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Loop pairs
    for (int p = 0; p < pair_count; p++)
    {    
        // assign variables for readability and convenience
        int wi = pairs[p].winner;
        int lo = pairs[p].loser;

        if (ccycle(wi, lo))
            locked[wi][lo] = 1;
    }
           
    // ctrl +++++++++++++++++++++++++++++++++++++++
    for (int v = 0; v < candidate_count; v++)
    {
        for (int z = 0; z < candidate_count; z++)
        {
            printf("locked[%i][%i] = %i\n", v, z, locked[v][z]);
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool beaten = false;
        for (int j = 0; j < candidate_count; j++)
        {    
            if (locked[j][i]) // -> any j is beating i?
            {
                beaten = true;
                break;
            }
        }
        if (!beaten)
            printf("%s\n", candidates[i]);
    }
    return;
}
