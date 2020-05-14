#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;

void* thread_runner(void*);
pthread_t tid1, tid2;
struct THREADDATA_STRUCT{
    pthread_t creator;

};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA* p=NULL;     //default


//A flag to indicate if the reading of input is complete,
//so the other thread knows when to stop
bool is_reading_complete = false;

//variable for indexing of messages by the logging function
int logindex=0;
int *logip = &logindex;

//for fgets...
char buffer[100];
char *result;


//functions
void logger(int* i, pthread_t m, THREADDATA* p);
/**********************************************************
 * function main  ------------------------------------------------- *********************************************************/
int main(){
    printf("create first thread\n");
    pthread_create(&tid1,NULL,thread_runner,NULL);
    printf("create second thread\n");
    pthread_create(&tid2,NULL,thread_runner,NULL);
    printf("wait for first thread to exit\n");

    //wait
    pthread_join(tid1,NULL);

    printf("first thread exited\n");
    printf("wait for second thread to exit\n");

    pthread_join(tid2,NULL);

    printf("second thread exited\n");
    exit(0);

}//end main


/**********************************************************************
function thread_runner runs inside each thread -------------------------------------------------- **********************************************************************/

void* thread_runner(void* x){
    pthread_t me;
    me = pthread_self();

    logger(logip, me, p);               //log
    printf("enter thread stream\n");


    pthread_mutex_lock(&tlock2);
    // critical section starts - malloc occurs if p = NULL
    if (p==NULL) {
        p = (THREADDATA*)malloc(sizeof(THREADDATA));
        p->creator=me;
        logger(logip, me, p);               //log
        printf("created THREADDATA\n");
    }
    pthread_mutex_unlock(&tlock2);  // critical section ends


    while ((result = fgets(buffer, 100, stdin)) != NULL) {
        if (*result == '\n')
            break;    //break immediately
        //first thread - check for string
        if (p!=NULL && p->creator==me) {

            //critical section of incrementation
            pthread_mutex_lock(&tlock1);
            logindex++;
            pthread_mutex_unlock(&tlock1);      //end crit section

            logger(logip, me, p);           //log
            printf("OK %s\n", buffer);
        }   //end if
            //TODO add to linked list node here, might need mutex?
            //is_reading_complete = true;     //set true
        else    {       //second thread: thread spinning
            //while(is_reading_complete == false); {
            sleep(0.2);    //wait 2s: reduce ttl CPU cycles
            //TODO print node
            printf("OK #2 %s\n", buffer);
            //}
        }
    }


    // TODO use mutex to make this a start of a critical section
    //pthread_mutex_lock(&tlock3);
    if (p!=NULL && p->creator==me)
        printf("This is thread %ld and I did not touch THREADDATA\n", me);
    else{
        /**   * TODO Free the THREADATA object. Freeing should be done by the other thread from the one that created it.
         * See how the THREADDATA was created for an example of how this is done.   */
        printf("This is thread %ld and I deleted the THREADDATA\n", me);
    }
    //pthread_mutex_unlock(&tlock3);

    pthread_exit(NULL);
    return NULL;

}//end thread_runner


void logger(int* i, pthread_t m, THREADDATA* p) {
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

    