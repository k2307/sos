#include <stdio.h>

#define P 5   // number of processes
#define R 3   // number of resource types

int alloc[P][R] = {{0,1,0}, {2,0,0}, {3,0,2}, {2,1,1}, {0,0,2}};
int max[P][R]   = {{7,5,3}, {3,2,2}, {9,0,2}, {2,2,2}, {4,3,3}};
int avail[R]    = {3, 3, 2};
int need[P][R];
int seq[P];

// Print the Allocation matrix, Need matrix and Available vector
void print_state() {
    printf("Process  Allocation  Need\n");
    for (int i = 0; i < P; i++) {
        printf("P%d       ", i);
        for (int j = 0; j < R; j++) printf("%d ", alloc[i][j]);
        printf("      ");
        for (int j = 0; j < R; j++) printf("%d ", need[i][j]);
        printf("\n");
    }
    printf("Available: ");
    for (int j = 0; j < R; j++) printf("%d ", avail[j]);
    printf("\n");
}

// Safety algorithm: can all processes finish in some order?
int is_safe() {
    int work[R], done[P] = {0}, count = 0;
    for (int j = 0; j < R; j++) work[j] = avail[j];

    while (count < P) {
        int found = 0;
        for (int i = 0; i < P; i++) {
            if (done[i]) continue;
            int ok = 1;
            for (int j = 0; j < R; j++)
                if (need[i][j] > work[j]) ok = 0;
            if (ok) {                                   // P[i] can finish
                for (int j = 0; j < R; j++) work[j] += alloc[i][j];
                done[i] = 1;
                seq[count++] = i;
                found = 1;
            }
        }
        if (!found) return 0;                           // nobody can finish: unsafe
    }
    return 1;
}

void print_seq() {
    printf("Safe sequence: ");
    for (int i = 0; i < P; i++) printf("P%d ", seq[i]);
    printf("\n");
}

// Grant a request only if the system stays in a safe state
void request(int p, int req[]) {
    printf("\nP%d requests (%d %d %d)\n", p, req[0], req[1], req[2]);

    for (int j = 0; j < R; j++)
        if (req[j] > need[p][j] || req[j] > avail[j]) {
            printf("Request denied: more than need or available\n");
            return;
        }

    for (int j = 0; j < R; j++) {                       // pretend to grant it
        avail[j] -= req[j];
        alloc[p][j] += req[j];
        need[p][j] -= req[j];
    }

    if (is_safe()) {
        printf("Request granted. ");
        print_seq();
    } else {
        for (int j = 0; j < R; j++) {                   // undo it
            avail[j] += req[j];
            alloc[p][j] -= req[j];
            need[p][j] += req[j];
        }
        printf("Request denied: system would become unsafe\n");
    }
    print_state();
}

int main() {
    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            need[i][j] = max[i][j] - alloc[i][j];

    printf("Initial state:\n");
    print_state();
    if (is_safe()) {
        printf("State is SAFE. ");
        print_seq();
    } else {
        printf("State is UNSAFE\n");
    }

    int r1[R] = {1, 0, 2};
    int r2[R] = {0, 2, 0};
    request(1, r1);
    request(0, r2);
    return 0;
}
