#include <stdio.h>

#define N 20   // length of reference string
#define F 3    // number of frames

int ref[N] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};

int find(int frames[], int page) {      // returns frame index, or -1 if not present
    for (int i = 0; i < F; i++)
        if (frames[i] == page) return i;
    return -1;
}

void init(int frames[]) {
    for (int i = 0; i < F; i++) frames[i] = -1;
}

int fifo() {
    int frames[F], next = 0, faults = 0;
    init(frames);
    for (int i = 0; i < N; i++)
        if (find(frames, ref[i]) == -1) {
            frames[next] = ref[i];      // replace the oldest page
            next = (next + 1) % F;
            faults++;
        }
    return faults;
}

int lru() {
    int frames[F], last_used[F], faults = 0;
    init(frames);
    for (int j = 0; j < F; j++) last_used[j] = -1;
    for (int i = 0; i < N; i++) {
        int pos = find(frames, ref[i]);
        if (pos == -1) {
            pos = 0;
            for (int j = 1; j < F; j++)         // pick least recently used
                if (last_used[j] < last_used[pos]) pos = j;
            frames[pos] = ref[i];
            faults++;
        }
        last_used[pos] = i;
    }
    return faults;
}

int optimal() {
    int frames[F], faults = 0;
    init(frames);
    for (int i = 0; i < N; i++) {
        if (find(frames, ref[i]) != -1) continue;
        int pos = -1, farthest = -1;
        for (int j = 0; j < F; j++) {
            if (frames[j] == -1) { pos = j; break; }
            int next = N;                       // N means "never used again"
            for (int k = i + 1; k < N; k++)
                if (ref[k] == frames[j]) { next = k; break; }
            if (next > farthest) { farthest = next; pos = j; }
        }
        frames[pos] = ref[i];                   // replace page used farthest in future
        faults++;
    }
    return faults;
}

void show(char *name, int faults) {
    printf("%-8s faults = %2d   hits = %2d   fault rate = %.1f%%\n",
           name, faults, N - faults, 100.0 * faults / N);
}

int main() {
    printf("Reference string length = %d, Frames = %d\n\n", N, F);
    show("FIFO", fifo());
    show("LRU", lru());
    show("Optimal", optimal());
    return 0;
}
