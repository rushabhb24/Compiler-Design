#include<stdio.h>
#include<string.h>
#define TSIZE 128

int table[100][100];
char terminal[TSIZE];
char nonterminal[26];

struct product {
    char str[100];
    int len;
} pro[20];

int no_pro;
char first[26][TSIZE];
char follow[26][TSIZE];
int getIdx[200];

// Check if the character is a non-terminal (uppercase)
int isNT(char c) {
    return c >= 'A' && c <= 'Z';
}

// Read production rules from a file
void readFromFile() {
    FILE* fptr;
    fptr = fopen("text.txt", "r");
    char buffer[255];
    int i, j;

    while (fgets(buffer, sizeof(buffer), fptr)) {
        printf("%s", buffer);
        j = 0;
        nonterminal[buffer[0] - 'A'] = 1;

        for (i = 0; i < strlen(buffer) - 1; ++i) {
            if (buffer[i] == '|') {
                ++no_pro;
                pro[no_pro - 1].str[j] = '\0';
                pro[no_pro - 1].len = j;
                pro[no_pro].str[0] = pro[no_pro - 1].str[0];
                pro[no_pro].str[1] = pro[no_pro - 1].str[1];
                pro[no_pro].str[2] = pro[no_pro - 1].str[2];
                j = 3;
            } else {
                pro[no_pro].str[j] = buffer[i];
                ++j;
                if (!isNT(buffer[i]) && buffer[i] != '-' && buffer[i] != '>') {
                    terminal[buffer[i]] = 1;
                }
            }
        }
        pro[no_pro].len = j;
        ++no_pro;
    }

    fclose(fptr);
}

// Add FIRST(A) to FOLLOW(B)
void add_FIRST_A_to_FOLLOW_B(char A, char B) {
    for (int i = 0; i < TSIZE; ++i) {
        if (i != '^') {
            follow[B - 'A'][i] = follow[B - 'A'][i] || first[A - 'A'][i];
        }
    }
}

// Add FOLLOW(A) to FOLLOW(B)
void add_FOLLOW_A_to_FOLLOW_B(char A, char B) {
    for (int i = 0; i < TSIZE; ++i) {
        if (i != '^') {
            follow[B - 'A'][i] = follow[B - 'A'][i] || follow[A - 'A'][i];
        }
    }
}

// Function to calculate FOLLOW sets
void FOLLOW() {
    int t = 0;
    int i, j, k, x;

    while (t++ < no_pro) {
        for (k = 0; k < 26; ++k) {
            if (!nonterminal[k]) continue;
            char nt = k + 'A';

            for (i = 0; i < no_pro; ++i) {
                for (j = 3; j < pro[i].len; ++j) {
                    if (nt == pro[i].str[j]) {
                        for (x = j + 1; x < pro[i].len; ++x) {
                            char sc = pro[i].str[x];
                            if (isNT(sc)) {
                                add_FIRST_A_to_FOLLOW_B(sc, nt);
                                if (first[sc - 'A']['^'])
                                    continue;
                            } else {
                                follow[nt - 'A'][sc] = 1;
                            }
                            break;
                        }

                        if (x == pro[i].len)
                            add_FOLLOW_A_to_FOLLOW_B(pro[i].str[0], nt);
                    }
                }
            }
        }
    }
}

// Add FIRST(A) to FIRST(B)
void add_First_A_to_FIRST_B(char A, char B) {
    for (int i = 0; i < TSIZE; ++i)
        if (i != '^')
            first[B - 'A'][i] = first[A - 'A'][i] || first[B - 'A'][i];
}

// Function to calculate FIRST sets
void FIRST() {
    int i, j;
    int t = 0;

    while (t < no_pro) {
        for (i = 0; i < no_pro; ++i) {
            for (j = 3; j < pro[i].len; ++j) {
                char sc = pro[i].str[j];
                if (isNT(sc)) {
                    add_First_A_to_FIRST_B(sc, pro[i].str[0]);
                    if (first[sc - 'A']['^'])
                        continue;
                } else {
                    first[pro[i].str[0] - 'A'][sc] = 1;
                }
                break;
            }

            if (j == pro[i].len)
                first[pro[i].str[0] - 'A']['^'] = 1;
        }
        ++t;
    }
}

int main() {
    readFromFile();
    follow[pro[0].str[0] - 'A']['$'] = 1;
    FIRST();
    FOLLOW();

    int i, j;
    printf("\n");

    // Print FIRST sets
    for (i = 0; i < no_pro; ++i) {
        if (i == 0 || (pro[i - 1].str[0] != pro[i].str[0])) {
            char c = pro[i].str[0];
            printf("FIRST OF %c: ", c);
            for (j = 0; j < TSIZE; ++j) {
                if (first[c - 'A'][j]) {
                    printf("%c ", j);
                }
            }
            printf("\n");
        }
    }

    printf("\n");

    // Print FOLLOW sets
    for (i = 0; i < no_pro; ++i) {
        if (i == 0 || (pro[i - 1].str[0] != pro[i].str[0])) {
            char c = pro[i].str[0];
            printf("FOLLOW OF %c: ", c);
            for (j = 0; j < TSIZE; ++j) {
                if (follow[c - 'A'][j]) {
                    printf("%c ", j);
                }
            }
            printf("\n");
        }
    }

    return 0;
}
