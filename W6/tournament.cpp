#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
typedef struct Team{
    int id;
    char name[20];
    int point;
    Team *left;
    Team *right;
} Team;
int size;
Team* createNode(Team team){
    Team* newNode;
    newNode = &team;
    newNode->point = team.point;
    return newNode;
}
void postOrderTraversal(Team *root,int level = 0) {
    if (root == NULL) {
        return;
    }
    
    postOrderTraversal(root->left,level + 1);
    
    postOrderTraversal(root->right, level + 1);
    
    if(root->left != NULL && root->right != NULL){
        printf("Round ");
        switch(level){
            case 0:
                printf("Final:        |");
                break;
            case 1:
                printf("Semi-Final:   |");
                break;
            case 2:
                printf("Quater-Final: |");
                break;
            case 3:
                printf("Round Of 16:  |");
                break;
        }
        printf("Match ID: %d | %s %d - %d  %s => Winner: %s \n",root->id,root->left->name,root->left->point,root->right->point,root->right->name,root->name);
    };
}
int random(int min, int max){
    return min + rand() % (max - min + 1);
}
Team findWinner(Team a, Team b){
    if(a.point > b.point){
        return a;
    } else {
        return b;
    }
}
void pathToWinner(Team *root,int level){
    if(root == NULL) return;
    if(root->left == NULL && root->right == NULL) return;
    Team *match,*team;
    if(strcmp(root->left->name,root->name) == 0){
        pathToWinner(root->left,level + 1); 
        match = root->right;
    } else {
        pathToWinner(root->right,level + 1);  
        match = root->left;
    }
    printf("Match ID: %d |",root->id);
    switch (level) {
        case 0: 
            printf("Final:         ");
            break;
        case 1:
            printf("Semi-Final:    "); 
            break;
        case 2: 
            printf("Quarter-Final: "); 
            break;
        case 3: 
            printf("Round of 16:   "); 
            break;
    }
    if( match == root->right){
        printf("%s %d - %d %s\n",root->left->name,root->left->point,match->point,match->name);       
    } else {
        printf("%s %d - %d %s\n",match->name,match->point,root->right->point,root->right->name);       
    }
}
Team* findPotentialMeeting(Team* root, char team1[], char team2[], int level) {
    if (root == NULL) return NULL;
    
    // Base case: Found a leaf node that matches one of our teams
    if (root->left == NULL && root->right == NULL) {
        if (strcmp(root->name, team1) == 0) {
            return root;  // Found team1
        }
        if (strcmp(root->name, team2) == 0) {
            return root;  // Found team2
        }
        return NULL;  // Leaf node but not our team
    }
    
    Team* leftFound = findPotentialMeeting(root->left, team1, team2, level + 1);
    Team* rightFound = findPotentialMeeting(root->right, team1, team2, level + 1);
    
    if (leftFound != NULL && rightFound != NULL) {
        printf("=== POTENTIAL MEETING ===\n");
        printf("Teams: %s vs %s\n", team1, team2);
        printf("Round: ");
        switch(level) {
            case 0: printf("Final"); break;
            case 1: printf("Semi-Final"); break;
            case 2: printf("Quarter-Final"); break;
            case 3: printf("Round of 16"); break;
            default: printf("Round %d", level);
        }
        printf("\nWould meet at Match ID: %d\n", root->id);
        printf("=========================\n");
        
        return root;
    }
    
    // Propagate whichever team we found upward
    return (leftFound != NULL) ? leftFound : rightFound;
}
void initializeTree(int level,int column,int maxLevel,Team **bracket,Team *root){
    if(level == maxLevel - 1){
        return;
    }
    root->left = &(bracket[level + 1][column*2]);
    root->right = &(bracket[level + 1][column*2 + 1]);
    initializeTree(level + 1,column*2,maxLevel,bracket,root->left);
    initializeTree(level + 1,column*2 + 1,maxLevel,bracket,root->right);
}
Team* initializeTour(Team tournament[]){
    int numLevel = ceil(log2(size)) + 1;
    int id = 1;
    int tourSize = pow(2,numLevel - 1);
    Team **tournamentBracket = (Team**)malloc(numLevel*sizeof(Team*));
    for(int i = 0 ; i < numLevel ; i++){
        tournamentBracket[i] = (Team*)malloc(tourSize*sizeof(Team));
    }
    // Team tournamentBracket[numLevel][size];
    for(int i = 0 ; i < size ; i++){
        tournamentBracket[numLevel - 1][i] = tournament[i];
    }
    for(int i = size; i < tourSize; i++) {
        tournamentBracket[numLevel - 1][i].id = 0;
        strcpy(tournamentBracket[numLevel - 1][i].name, "BYE");
        tournamentBracket[numLevel - 1][i].point = 0;
    }
    for(int i = numLevel - 1 ; i > 0 ; i--){
        for(int j = 0 ; j < pow(2,i) ; j += 2){
            if(strcmp(tournamentBracket[i][j + 1].name,"BYE") == 0){
                strcpy(tournamentBracket[i - 1][j/2].name,tournamentBracket[i][j].name);
                tournamentBracket[i - 1][j/2].id = -1;
            } else {
                tournamentBracket[i][j].point = random(0,5);
                tournamentBracket[i][j + 1].point = random(0,5);
                if(tournamentBracket[i][j].point == tournamentBracket[i][j + 1].point){
                    tournamentBracket[i][j].point++;
                }
                strcpy(tournamentBracket[i - 1][j/2].name,findWinner(tournamentBracket[i][j],tournamentBracket[i][j + 1]).name);
                tournamentBracket[i - 1][j/2].id = id;
                id++;
            }
        }
    }
    tournamentBracket[0][0].point = -1;
    Team *champion = &tournamentBracket[0][0];
    initializeTree(0,0,numLevel,tournamentBracket,champion);

    // Free Allocated Memory
    // for(int i = 0 ; i < numLevel ; i++){
    //     free(tournamentBracket[i]);
    // }
    // free(tournamentBracket); 

    
    return champion;
}
int main(){
    srand(time(NULL));
    Team tour[] ={{0,"Argentina",0},{0,"Portugal",0},{0,"Spain",0},{0,"Brazil",0},{0,"Netherland",0}};
    size = sizeof(tour)/sizeof(tour[0]);
    Team *root = initializeTour(tour);
    postOrderTraversal(root);
    findPotentialMeeting(root,"Portugal","Brazil",0);
    pathToWinner(root,0);
    return 0;
}