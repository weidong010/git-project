
#include <stdlib.h>
#include"L_Heartbeat_Interface.h"

/* heartbeat_log_t()        :  assign a buffer to save the log
 * @ buffer_size            :  number of heartbeat log unit in memory
 * @ pid                    :  pthread number
 */

static inline heartbeat_log_t* L_Heartbeat_Alloc_Log(int64_t size) {
     
  heartbeat_log_t* p = NULL;
  
  p = malloc(size*sizeof(heartbeat_log_t));   //assign number of "buffer size" the type of heartbeat_log memory;

  return p;                                          //return the first address of the memory;
}

static inline heartbeat_memory_t* L_Heartbeat_Alloc_Memory(int64_t size) {
     
  heartbeat_log_t* p = NULL;
  
  p = malloc(size*sizeof(heartbeat_memory_t));   //assign number of "buffer size" the type of heartbeat_memory_t memory;

  return p;                                          //return the first address of the memory;
}

heartbeat_memory_t* L_Heartbeat_Alloc_Global_Matrix(int64_t num){
    
   heartbeat_memory_t* p = NULL;
   while(1){
 
          p = malloc(num*sizeof(heartbeat_memory_t));
          if(p){
              break;
          } 
   }  
   return p;
}  

/* L_Heartbeat_flu3sh_buffer()        :  flush the memory data to log
 * @ heartbeat_profile_t             :  a heartbeat profile
 */

static void L_Heartbeat_flush_buffer(heartbeat_profile_t volatile * hb) {
  long i;
  long numb = hb->buffer_to_file;                             // total number of buffer chunks;`
  if(hb->text_file != NULL) {
    for(i = 0; i < numb; i++) {
      fprintf(hb->text_file,                                  // write info in memory to log;
	      "%ld    %d    %lld    %lf   \n", 
	       hb->log[i].beat,                               // write ;the current heartbeat info, beat, to log;
	       hb->log[i].tag,                                // write the current heartbeat info, tag, to log;
	       (long long int) hb->log[i].timestamp,           // write the current heartbeat info, timestamp, to log;
	       hb->log[i].heartbeat_rate);                    // write the current heartbeat info, heartbeat rate, to log;
    }   
    fflush(hb->text_file);                                   // flush the input memory
  }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
unsigned int log_inc=0;   // global variable as a counter of items in log.

/* L_Heartbeat_Init()    :  Initialize a heartbeat in an application loop
 *  * @ long max            :  maximum number of heartbeats in a group
 * @ heartbeat_profile_t :  Initialize a heartbeat
 * @ long interval       :  interval between two beats (nanosecond)
 * 
 */

int L_Heartbeat_Init(heartbeat_profile_t * hb, heartbeat_sequence* hs,long max, long interval){
  int rc = 0;     //file-memory operation. 0-success; 1-pthread_ID NULL failure; 2-failure, assign memory failure; 
  hb->interval=interval; //interval between two beats (nanosecond);
  hb->max=max;           //maximum number of heartbeats;
  printf("thread =%u---------here\n", (unsigned int)pthread_self());
  hb->tid = pthread_self();  //get the thread number;
  int key = (int)hb->tid;    //use the thread as a keyword for request the memory;
  long size = 50;      

  hb->log = L_Heartbeat_Alloc_Log(size);  //assign a memory space for heartbeat;
  hb->memory= L_Heartbeat_Alloc_Memory(max);

  if(hb->log == NULL){
    rc = 2;                      //log name missing;
  }
    char pthread_ID[300];          // save the name of thread number;
    sprintf(pthread_ID, "%u%u",log_inc,(unsigned)hb->tid);   //write the thread name to the "pthread_ID";
    if (hb->text_file = fopen(pthread_ID, "w")){
    fprintf(hb->text_file, "Beat    Tag    Timestamp    Heartbeat Rate \n" ); //write the name to the sheet header;
    log_inc++;   //log calculate.
    }
    else{
    hb->text_file = NULL;                       //open file failed;
    rc=1;
    }
  hb->first_timestamp = hb->last_timestamp = 0; //initialize the timestamp for the first time;
  hb->current_index = 0;                        //initialize the current_index for the first time;
  hb->counter = 0;                              //initialize the counter for the first time;
  hb->buffer_index = 0;                         //initialize the buffer_index for the first time;
  hb->buffer_to_file = size;                    //initialize the buffer_depth for the first time;
  pthread_mutex_init(&hb->mutex, NULL);         //initialize the mutex for the first time;
  hs->amount=0;
  hs->tid=hb->tid;
  pthread_mutex_init(&hs->mutex, NULL);
  hs->p->heartbeat_rate=1.0;
  printf("ps->p->heartbeat_rate=%lf",hs->p->heartbeat_rate);
  printf("hs->p=%d",hs->p);
  return rc;                                    
}

/* L_Heartbeat_Generate() : Create a heartbeat and log.
 * 
 * @ heartbeat_profile_t  : the heartbeat initialized 
 * @ int tag              : tag used for recognizing each heartbeat 
 * @ int cycle            : used for inside loop times, and the value is the same as loop times outside
 * @ int frequency        : adjust number of heartbeats for example the value cycle/frequency denotes number of heartbeats
 */

int64_t L_Heartbeat_Generate(heartbeat_profile_t* hb, heartbeat_sequence* hs,int tag, int cycle, int frequency){
    struct timespec time_info;                // define time_info for save current time info;
    int64_t time;                             // save the time as time point (nanosecond);
    int index;   
    int counter;
    long j;
    hb->frequency=frequency;                  // control the number of heartbeats in a loop, and 2 denotes generating half of loop times;
    hb->cycle=cycle;                          // total loop times; 
    clock_gettime( CLOCK_REALTIME, &time_info );  //get the current time;
    time = ( (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec ); // save the time as time point (nanosecond);
    pthread_mutex_lock(&hb->mutex);                                                   
    hb->last_timestamp = time;                                                        // save the last time info;
    if(hb->first_timestamp == 0) {
      index = 0;                                                                      // deal with a heartbeat for the first timestamp;
      hb->first_timestamp = time;                                                     // timestamp for the first time;
      hb->last_timestamp  = time;                                                     // timestamp for the previous time;
      hb->log[0].beat = 0;                                                            // write a beat as log for the first timestamp;
      hb->log[0].tag = tag;                                                           // write a tag as log for the first timestamp;
      hb->log[0].timestamp = time;                                                    // write a timestamp as log for the first timestamp;
      hb->log[0].heartbeat_rate = 0;                                                  // write a heartbeat rate as log for the first timestamp;
      hb->memory[0].timestamp=time;
      hb->memory[0].heartbeat_rate=0;
      hb->memory[0].tid=hb->tid;
      hs->p[0].heartbeat_rate=0.0;
      hs->p[0].timestamp=0;
      hs->p[0].index=1;
      hs->tid=hb->tid;
      hs->last=hs->p;
      hs->amount++;
      printf("pt->p[0].heartbeat_rate=%lf",hs->p[0].heartbeat_rate);
      printf("pt->p=%d",hs->p);
      hb->counter++;                                                                  // heartbeats counter;
      hb->buffer_index++;                                                             // buffer index;
    }
    else {                                                                            // redefine number of heartbeats; 
        if(hb->cycle % hb->frequency==0){                                             // cycle is total number of heartbeats;
      hb->last_timestamp = time;                                                      // previous the timestamp;
      double heartbeat_rate = (((double) hb->counter) / ((double) (time - hb->first_timestamp)))*1000000000.0;  //function for calculate the heartbeat rate;
      index =  hb->buffer_index;  
      counter = hb->counter;
      hb->log[index].beat = hb->counter;                                              // prepared to save the current heartbeat info, beat, to log.
      hb->log[index].tag = tag;                                                       // prepared to save the current heartbeat info, tag, to log.
      hb->log[index].timestamp = time;                                                // prepared to save the current heartbeat info, timestamp, to log.  
      hb->log[index].heartbeat_rate = heartbeat_rate;                                 // prepared to save the current heartbeat info, heartbeat rate, to log.
      hb->memory[counter].timestamp= time;
      hb->memory[counter].heartbeat_rate = heartbeat_rate;
      hb->memory[counter].tid=hb->tid;
      hs->p[counter].heartbeat_rate=heartbeat_rate;
      hs->p[counter].timestamp=time;
      hs->tid=hb->tid;
      pthread_mutex_lock(&hs->mutex); 
      hs->last=&(hs->p[counter]);
      pthread_mutex_unlock(&hs->mutex);
      hs->amount++;
      hs->p[counter].index=hs->amount;
      hb->buffer_index++;                                                             
      hb->counter++;
      if(hb->buffer_index%hb->buffer_to_file == 0) {                                    //write the log to the file                         
	if(hb->text_file != NULL)
	   L_Heartbeat_flush_buffer(hb);       
	   hb->buffer_index = 0;
      }
    }
    }
    pthread_mutex_unlock(&hb->mutex);  
    usleep(hb->interval);                                                              // sleep for a interval
      return time;
}

/* L_Heartbeat_End()      : quit a heartbeat, close the log file, and free memory
 * 
 * @ heartbeat_profile_t  : the heartbeat initialized 
 */
/*
int L_Heartbeat_MainThread_LastItemInLog(){
   void *status=NULL;
   heartbeat_profile_t *t;
}*/

void L_Heartbeat_End(heartbeat_profile_t * hb){                                      
   if(hb->log != NULL){                                                              //free the log memory;
       free(hb->log);          
   }
   if(hb->memory !=NULL){
       free(hb->memory);
   }
    if(hb->text_file != NULL)                                                        // close the text_file;
    fclose(hb->text_file);
}

int L_Hearteat_Monitor_A(heartbeat_sequence* ht,int num_thread,long heartbeat_amount,long heartbeat_frequency){
    int FLAG=0;
    int i;
    int num=num_thread;
    long amount=heartbeat_amount;
    long frequency=heartbeat_frequency;   
    long real_amount=amount/frequency;
    double test_interval;
    struct timespec time_info;                // define time_info for save current time info;
    int64_t time;                             // save the time as time point (nanosecond);   
    for(i=0;i<num;i++){
    printf("arg[%d].amount=%ld\n",i,ht[i].amount);
    if (ht[i].amount == real_amount){      
    FLAG=1;
    clock_gettime( CLOCK_REALTIME, &time_info );  //get the current time;
    time = ( (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec ); // save the time as time point (nanosecond);    
    test_interval = (double) (time - ht[i].last->timestamp);  //function for calculate the heartbeat rate;
    printf("test_interval= %lf\n",test_interval);
    printf("ht[%d].amount=%d\n",i,ht[i].amount);
    }      
    else if (test_interval>100000000) {
        FLAG=2;
        printf("Thread %d is abnormal, the thread number is %lu\n",i,ht[i].tid);   
    }   
    else{
    FLAG=2;   
    printf("Thread %lu (Number is %lu) is on a exeception status. The total number of heartbeats is less than the number of %ld\n", i, ht[0].tid, real_amount);
    }
    }     
    return FLAG;
}