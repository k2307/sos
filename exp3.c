#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>

/*
 * IPC DEMO: Shared Memory + Message Queue
 *
 * Flow of the program:
 *   1. Parent creates a shared memory segment and a message queue.
 *   2. Parent calls fork() to create a child process.
 *   3. Child writes data into shared memory and sends a message.
 *   4. Parent waits for the child, then reads the shared memory
 *      and receives the message.
 *   5. Parent deletes both IPC resources.
 */

// Every message queue message must start with a 'long' type (> 0),
// followed by the actual data.
struct msg {
    long type;          // message type, used to pick which message to receive
    char text[50];      // message data
};

int main() {
    // STEP 1: Create the IPC resources (done BEFORE fork so both
    // processes know the ids).

    // shmget() creates a 100-byte shared memory segment and returns its id.
    // IPC_PRIVATE = make a new, unique segment
    // IPC_CREAT | 0666 = create it, with read/write permission for all
    int shmid = shmget(IPC_PRIVATE, 100, IPC_CREAT | 0666);

    // msgget() creates a message queue and returns its id.
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    // STEP 2: fork() makes a child process.
    // The child gets a copy of shmid and msqid, so it can use the same
    // shared memory and queue. fork() returns 0 inside the child.
    if (fork() == 0) {

        // ---------- CHILD PROCESS (the sender) ----------

        // STEP 3a: shmat() attaches (maps) the shared memory into this
        // process's address space and returns a pointer to it.
        // Anything written here is visible to the parent too.
        char *shm = shmat(shmid, NULL, 0);
        strcpy(shm, "Hello via shared memory!");

        // STEP 3b: msgsnd() copies a message into the queue (kept by the
        // kernel). The size is the size of the data only, not the 'type'.
        struct msg m = {1, "Hello via message queue!"};
        msgsnd(msqid, &m, sizeof(m.text), 0);

        // STEP 3c: shmdt() detaches the memory from this process.
        // (It does NOT delete the segment.)
        shmdt(shm);
        return 0;       // child exits
    }

    // ---------- PARENT PROCESS (the receiver) ----------

    // STEP 4a: Wait for the child to finish.
    // Shared memory has no built-in synchronization, so we wait here to
    // make sure the child has finished writing before we read.
    wait(NULL);

    // STEP 4b: Attach the same shared memory and read what the child wrote.
    char *shm = shmat(shmid, NULL, 0);
    printf("Shared memory: %s\n", shm);

    // STEP 4c: msgrcv() takes a message of type 1 out of the queue.
    // If no message is available, it blocks (waits) until one arrives.
    struct msg m;
    msgrcv(msqid, &m, sizeof(m.text), 1, 0);
    printf("Message queue: %s\n", m.text);

    // STEP 5: Clean up. IPC resources stay in the kernel even after the
    // program ends, so we must remove them ourselves (check with `ipcs`).
    shmdt(shm);                         // detach shared memory
    shmctl(shmid, IPC_RMID, NULL);      // delete shared memory
    msgctl(msqid, IPC_RMID, NULL);      // delete message queue
    return 0;
}
