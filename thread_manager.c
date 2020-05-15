#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "CommandNode.h"

///////////////////////////////////////////////////////////////////////////////////////////
// TODO: read in stdin and store into the head of linked list.
//thread mutex lock for access to the log index.
// sample output: "Logindex 1, thread 2, PID 5435, DATE TIME: Head of the linked list contains line foo."
//thread mutex lock for log index incrementation
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;

//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of deallocating THREADDATA
pthread_mutex_t tlock22 = PTHREAD_MUTEX_INITIALIZER;

//thread mutex lock for is_reading_complete flag
pthread_mutex_t tlock3= PTHREAD_MUTEX_INITIALIZER;

//thread mutex lock for head command
pthread_mutex_t lockhead = PTHREAD_MUTEX_INITIALIZER;

pthread_t tid1, tid2;
struct THREADDATA_STRUCT
{
    pthread_t creator;
};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA* p=NULL;


//variable for indexing of messages by the logging function
int logindex=0;
int *logip = &logindex;


//variables to store user input
char buffer[100];
char *result;


//A flag to indicate if the reading of input is complete,
//so the other thread knows when to stop
bool is_reading_complete = false;


//functions
void setLog(int* i, pthread_t m, THREADDATA* p);
void* thread_runner(void* x);


/*********************************************************
// function main  -------------------------------------------------
*********************************************************/
int main() {
    printf("create first thread\n");
    pthread_create(&tid1,NULL,thread_runner,NULL);

    printf("create second thread\n");
    pthread_create(&tid2,NULL,thread_runner,NULL);

    printf("wait for first thread to exit\n");
    pthread_join(tid1,NULL);
    printf("first thread exited\n");

    printf("wait for second thread to exit\n");
    pthread_join(tid2,NULL);
    printf("second thread exited\n");

    exit(0);

}//end main

/**********************************************************************
// function thread_runner runs inside each thread --------------------------------------------------
**********************************************************************/
void* thread_runner(void* x)
{
    pthread_t me;
    me = pthread_self();

    pthread_mutex_lock(&tlock2); // critical section starts

    //log entry in thread stream
    setLog(logip, me, p);
    printf(" enter thread stream\n");

    if (p==NULL) {
        p = (THREADDATA*) malloc(sizeof(THREADDATA));
        p->creator=me;
        //log THREADDATA creation
        setLog(logip, me, p);
        printf(" created THREADDATA\n");
    }
    pthread_mutex_unlock(&tlock2);  // critical section end

    //node head creation
    CommandNode *head = (CommandNode *)malloc(sizeof(CommandNode));
    CreateCommandNode(head, buffer, logindex, NULL);

    //thread#1 block
    if (p!=NULL && p->creator==me) {
        /* this code block here should read user input and store it in variable result and store it into
           the node head.*/
        while ((result = fgets(buffer, 100, stdin)) != NULL) {
            pthread_mutex_lock(&lockhead);
            strcpy(head->command, result);     //change thread2
            setLog(logip, me, p);
            printf("Head node refreshed\n");
            pthread_mutex_unlock(&lockhead);
            if (*result == '\n') {
                pthread_mutex_lock(&tlock3);
                is_reading_complete = true;
                pthread_mutex_unlock(&tlock3);
                break;
            }

        }

        //printf("This is thread %ld and I created the THREADDATA %p\n",me,p);
    }   //end thread#1

    else {
        while (!is_reading_complete) {
            sleep(2);           //delay 2s: reduce CPU cycles
            if (strcmp(head->command, buffer) != 0 && strcmp(buffer, "\n") != 0) {       //if change in head command...
                pthread_mutex_lock(&lockhead);  //lock
                strcpy(head->command, result);  //copy via thread2
                //log changes in head node
                setLog(logip, me, p);
                printf("Head of linked list contains line : %s", head->command);
                pthread_mutex_unlock(&lockhead);
            }
        }
        // before printing any log messages, mutex_lock(locklogindex) so it does not mess up the counting.
        // before updating the head from thread 1 use mutex_lock(lockhead) and then mutex_unlock(lockhead).
        // something like while (!is_read_complete) { sleep(2);
        // check if head is different from last time, if yes then print the head.
        // print out if different from head?
    }


    // TODO use mutex to make this a start of a critical section
    pthread_mutex_lock(&tlock22);
    if (p!=NULL && p->creator==me) {
        free(head);
        setLog(logip, me, p);
        printf("deallocation of head node\n");
    }
    else {
        free(head);
        setLog(logip, me, p);
        printf("deallocation of head node\n");

        free(p);
        p = NULL;
        setLog(logip, me, p);
        printf("deleted p data\n");

    }
    pthread_mutex_unlock(&tlock22);// TODO critical section ends

    setLog(logip, me, p);
    printf("exiting thread runner\n");
    pthread_exit(NULL);
    return NULL;

} //end thread_runner


void setLog(int* i, pthread_t m, THREADDATA* p) {
    // variables to store date and time components
    int hours, minutes, seconds, day, month, year;

    // time_t is arithmetic time type
    time_t now;
    // localtime converts a time_t value to calendar time and
    // returns a pointer to a tm structure with its members
    // filled with the corresponding values
    struct tm *local;

    // Obtain current time
    // time() returns the current time of the system as a time_t value
    time(&now);
    //acquire current time
    local = localtime(&now);


    //get time
    hours = local->tm_hour;        // get hours since midnight (0-23)
    minutes = local->tm_min;       // get minutes passed after the hour (0-59)
    seconds = local->tm_sec;       // get seconds passed after minute (0-59)

    //get date
    day = local->tm_mday;          // get day of month (1 to 31)
    month = local->tm_mon + 1;     // get month of year (0 to 11)
    year = local->tm_year + 1900;  // get year since 1900

    //critical section of incrementation
    pthread_mutex_lock(&tlock1);
    printf("Logindex %d, thread %ld (p=%p), PID %d, %02d/%02d/%d %02d:%02d:%02d: ", ++*i, m, p, getpid(), month, day, year, hours, minutes, seconds);
    pthread_mutex_unlock(&tlock1);      //end crit section

    //printf("%02d/%02d/%d %02d:%02d:%02d: ", month, day, year, hours, minutes, seconds);

}
