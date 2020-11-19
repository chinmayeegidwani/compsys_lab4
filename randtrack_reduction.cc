
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
hash<sample,unsigned> h[4];

// initialize list of mutexes
pthread_mutex_t mutex[RAND_NUM_UPPER_BOUND];
void* process_one_seed(void* seed);
void* process_two_seeds(void* seed);
pthread_t thread1, thread2, thread3, thread4;

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

  printf("look for program arguments");
  // Parse program arguments
  /*if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  } */
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  //h.setup(14);

  int seed[4] = {0, 1, 2, 3};



  if(num_threads==1){ //business as usual
      h[0].setup(14);
      printf("business as usual");
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

          // if this sample has not been counted before
          if (!(s = h[0].lookup(key))){
      
      // insert a new element for it into the hash table
      s = new sample(key);
      h[0].insert(s);
          }

          // increment the count for the sample
          s->count++;

        }


    }
  } else if(num_threads==2){
    // call 2 seed func twice
      printf("creating thread");
      pthread_create(&thread1, NULL, process_two_seeds, &seed[0]);
      pthread_create(&thread1, NULL, process_two_seeds, &seed[1]);

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
      
  } else if(num_threads == 4){
      pthread_create(&thread1, NULL, process_one_seed, &seed[0]);
      pthread_create(&thread1, NULL, process_one_seed, &seed[1]);
      pthread_create(&thread1, NULL, process_one_seed, &seed[3]);
      pthread_create(&thread1, NULL, process_one_seed, &seed[4]);
    //call 1 seed func 4 times

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
      pthread_join(thread3, NULL);
      pthread_join(thread4, NULL);
  }



  // print a list of the frequency of all samples
  h.print();
}

void* process_one_seed(void* seed){
    // process streams starting with different initial numbers
  int seed_int = *((int *) seed);
  int rnum = seed_int;
  unsigned key;
  sample *s;
  h[seed_int].setup(14);

  // collect a number of samples
  for (int j=0; j<SAMPLES_TO_COLLECT; j++){

    // skip a number of samples
    for (int k=0; k<samples_to_skip; k++){
      rnum = rand_r((unsigned int*)&rnum);
    }

    // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
    key = rnum % RAND_NUM_UPPER_BOUND;


    // if this sample has not been counted before
    if (!(s = h[seed_int].lookup(key))){

        // insert a new element for it into the hash table
        s = new sample(key);
        h[seed_int].insert(s);
    }

    // increment the count for the sample
    s->count++;

  }
  return seed;
}



void* process_two_seeds(void* seed){
  int seed_int = *((int *) seed);
  int rnum = seed_int;
  unsigned key;
  sample *s;
  h[seed_int].setup(14);


  for(int i=0; i<2; i++){
    // collect a number of samples
    for (int j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (int k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      // if this sample has not been counted before
      if (!(s = h[seed_int].lookup(key))){

          // insert a new element for it into the hash table
          s = new sample(key);
          h[seed_int].insert(s);
      }

      // increment the count for the sample
      s->count++;

    }
    rnum++;
  }
  return seed;
}

