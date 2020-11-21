
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


void* process_seeds(void* args);
pthread_t thread1, thread2, thread3, thread4;

typedef struct threadInfo{
  int start_seed;
  int end_seed;
  hash<sample, unsigned> * thread_hash;
} threadInfo;

hash<sample,unsigned> h1;
hash<sample,unsigned> h2;
hash<sample,unsigned> h3;
hash<sample,unsigned> h4;
hash<sample,unsigned> h;

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
          // if this sample has not been counted before
          if (!(s = h.lookup(key))){
      
            // insert a new element for it into the hash table
            s = new sample(key);
            h.insert(s);
          }

          // increment the count for the sample
          s->count++;
        }


    }
  } else if(num_threads==2){
    // call seed func twice
      threadInfo t1;
      t1.start_seed = 0;
      t1.end_seed = 1;
      h1.setup(14);
      t1.thread_hash = &h1;

      threadInfo t2;
      t2.start_seed = 2;
      t2.end_seed = 3; 
      h2.setup(14);
      t2.thread_hash = &h2;

      pthread_create(&thread1, NULL, process_seeds, &t1);
      pthread_create(&thread2, NULL, process_seeds, &t2); 

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);

  } else if(num_threads == 4){
      threadInfo t1;
      t1.start_seed = 0;
      t1.end_seed = 0;
      h1.setup(14);
      t1.thread_hash = &h1;

      threadInfo t2;
      t2.start_seed = 1;
      t2.end_seed = 1;
      h2.setup(14);
      t2.thread_hash = &h2;

      threadInfo t3;
      t3.start_seed = 2;
      t3.end_seed = 2;
      h3.setup(14);
      t3.thread_hash = &h3;

      threadInfo t4;
      t4.start_seed = 3;
      t4.end_seed = 3; 
      h4.setup(14);
      t4.thread_hash = &h4;

      pthread_create(&thread1, NULL, process_seeds, &t1);
      pthread_create(&thread2, NULL, process_seeds, &t2);
      pthread_create(&thread3, NULL, process_seeds, &t3);
      pthread_create(&thread4, NULL, process_seeds, &t4);
    //call seed func 4 times

      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
      pthread_join(thread3, NULL);
      pthread_join(thread4, NULL);
  }

  /* stitch the tables back together
   * iterate through samples
   * check if sample is in the thread table
   * then update in global hash table
   */


  // ignore if num_threads = 1
  sample *thread_s;

  for(int i=0; i<RAND_NUM_UPPER_BOUND; i++){
    if(num_threads == 2){
      if(thread_s = h1.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }

      if(thread_s = h2.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }
    }

    if(num_threads == 4){
      if(thread_s = h1.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }

      if(thread_s = h2.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }

      if(thread_s = h3.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }

      if(thread_s = h4.lookup(i)){
        // if this number exists in the thread table

        if(!(s = h.lookup(i))){
          // if this num does not exist in the main table, add
          s = new sample(i);
          h.insert(s);
        }
        s->count += thread_s -> count;
      }
    }
  }

  // print a list of the frequency of all samples
  h.print();
}

void* process_seeds(void* args)
{
  threadInfo* thread_info = (threadInfo*) args;
  int start = thread_info -> start_seed;
  int end = thread_info -> end_seed;
  unsigned key;
  sample *s, *global_s;
  int rnum;

  for(int i=start; i<=end; i++){
    rnum = i;
    // collect a number of samples
    for (int j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (int k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      /******************* CRIT SECTION BEGIN *****************/
      // if this sample has not been counted before
      if (!(s = thread_info->thread_hash->lookup(key))){

          // insert a new element for it into the hash table
          s = new sample(key);
          thread_info->thread_hash->insert(s);
          /*
          if(!(global_s=h.lookup(i))){
              // if this element does not already exist in global hash table, add
              global_s = new sample(key);
              h.insert(global_s);
              global_s -> count++;
          } */
      }

      // increment the count for the sample
      s->count++;
      //global_s -> count++;
      /*********************CRIT SECTION END *********************/
    }
  }

}