
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "N-Synch",                  /* Team name */

    "Chinmayee Gidwani",                    /* Member full name */
    "1003062473",                 /* Member student number */
    "chinmayee.gidwani@mail.utoronto.ca",                 /* Member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* process_one_seed(void* seed);
void* process_seeds(void* args);
pthread_t thread1, thread2, thread3, thread4;

typedef struct threadInfo{
  int start_seed;
  int end_seed;
} threadInfo;

int main (int argc, char* argv[]){
  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);

  int seed[4] = {0, 1, 2, 3};




  if(num_threads==1){ //business as usual
    // process streams starting with different initial numbers
      for (i=0; i<NUM_SEED_STREAMS; i++){
        rnum = i;

        // collect a number of samples
        for (j=0; j<SAMPLES_TO_COLLECT; j++){

          // skip a number of samples
          for (k=0; k<samples_to_skip; k++){
      rnum = rand_r((unsigned int*)&rnum);
          }

          // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
          key = rnum % RAND_NUM_UPPER_BOUND;
        pthread_mutex_lock(&mutex);
          // if this sample has not been counted before
          if (!(s = h.lookup(key))){
      
            // insert a new element for it into the hash table
            s = new sample(key);
            h.insert(s);
          }

          // increment the count for the sample
          s->count++;
          pthread_mutex_unlock(&mutex);
        }


    }
  } else if(num_threads==2){
    // call 2 seed func twice

      threadInfo t1;
      t1.start_seed = 0;
      t1.end_seed = 1;

      threadInfo t2;
      t1.start_seed = 2;
      t1.end_seed = 3;

      //printf("creating thread");
      pthread_create(&thread1, NULL, process_seeds, &t1); // 0, 1
      pthread_create(&thread2, NULL, process_seeds, &t2); // 2, 3

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
  } else if(num_threads == 4){
    /*
      pthread_create(&thread1, NULL, process_one_seed, &seed[0]);
      pthread_create(&thread2, NULL, process_one_seed, &seed[1]);
      pthread_create(&thread3, NULL, process_one_seed, &seed[3]);
      pthread_create(&thread4, NULL, process_one_seed, &seed[4]);
    //call 1 seed func 4 times

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
      pthread_join(thread3, NULL);
      pthread_join(thread4, NULL); */
  }



  // print a list of the frequency of all samples
  h.print();
}

void* process_seeds(void* args){ //0
  threadInfo* thread_info = (threadInfo*) args;
  int start = thread_info -> start_seed;
  int end = thread_info -> end_seed;
  unsigned key;
  sample *s;
  int rnum;

  for(int i=start; i<end; i++){
    rnum = i;
    // collect a number of samples
    for (int j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (int k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      pthread_mutex_lock(&mutex); //lock before using hash table
      // if this sample has not been counted before
      if (!(s = h.lookup(key))){

          // insert a new element for it into the hash table
          s = new sample(key);
          h.insert(s);
      }

      // increment the count for the sample
      s->count++;
      pthread_mutex_unlock(&mutex);
    }
  }
  return (void*) 0;
}

