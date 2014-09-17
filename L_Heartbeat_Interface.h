/* 
 * File:   L_Heartbeat_Interface.h
 * Author: weidong
 *
 * Created on August 13, 2014, 6:14 PM
 */

#ifndef L_HEARTBEAT_RING_H
#define	L_HEARTBEAT_RING_H


#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <libgomp_g.h>

typedef struct {
  long beat;                 // Heartbeat 
  int  tag;                   // used for recongnizing each beat
  long timestamp;            // used for record each beat time
  double heartbeat_rate;     // refeclt the beat frequency
} heartbeat_log_t;           // struct used for record each beat data

typedef struct {
  long index;
  pthread_t tid; 
  double heartbeat_real_value;
  double heartbeat_rate;
  long timestamp;
} heartbeat_memory_t;

typedef struct {
    long amount;
    pthread_t tid;
    heartbeat_memory_t* p;     // Pointer to the unit of memory. 
    heartbeat_memory_t* last;     // Pointer to the unit of memory.
    pthread_mutex_t mutex;     // Mutex for some memory operations. 
} heartbeat_sequence;

typedef struct {
  pthread_t tid;             // used for multi-threads as saving a thread number
  long counter;               // beat counter
  long buffer_to_file;       // at one time write how many buffer size to file
  long buffer_index;         // buffer index
  int64_t first_timestamp;   // timestamp for last time
  int64_t last_timestamp;    // timestamp for current time
  long current_index;        // current index
  long max;                  // max number of heartbeats in a group
  long interval;             // interval between two beats (nanosecond)
  int frequency;             // input 2, 4, 5, or ... n denotes total number/n times
  int cycle;                 // used for total number of loop times
  heartbeat_log_t* log;      // pointer for log
  heartbeat_memory_t* memory; //pointer for memory
  FILE* text_file;           // pointer to file saving heartbeat record.
  pthread_mutex_t mutex;     // mutex for some memory operations. 
} heartbeat_profile_t;       // heartbeat profile.

#ifdef	__cplusplus
extern "C" {

heartbeat_memory_t* L_Heartbeat_Alloc_Global_Matrix(int64_t row, int64_t column);


/* L_Heartbeat_Init()    :  Initialize a heartbeat in an application loop.
 * 
 * @ heartbeat_profile_t :  Initialize a heartbeat
 * @ long max            :  max number of heartbeats in a group
 * @ long interval       :  interval between two beats (nanosecond)
 * 
 */
int L_Heartbeat_Init(heartbeat_profile_t * hb,long max,long interval);


/* L_Heartbeat_Generate() : Create a heartbeat and log.
 * 
 * @ heartbeat_profile_t  : the heartbeat initialized 
 * @ int tag              : tag used for recognizing each heartbeat 
 * @ int cycle            : used for inside loop times, and the value is the same as loop times outside
 * @ int frequency        : adjust number of heartbeats for example the value cycle/frequency denotes number of heartbeats
 */

int64_t L_Heartbeat_Generate(heartbeat_profile_t* hb, heartbeat_sequence* hs, int tag, int cycle, int frequency);

/* L_Heartbeat_End()      : quit a heartbeat, close the log file, and free memory
 * 
 * @ heartbeat_profile_t  : the heartbeat initialized 
 */

void L_Heartbeat_End(heartbeat_profile_t * hb);

/* L_Hearteat_Monitor()      : test threads and report its status. 
 * 
 * @ heartbeat_profile_t  : the heartbeat initialized 
 */

int L_Hearteat_Monitor_A(heartbeat_sequence* ht,int num_thread,long heartbeat_amount,long heartbeat_frequency);

int L_Hearteat_Monitor_B(heartbeat_sequence* ht,int num_thread,long heartbeat_amount,long heartbeat_frequency);

#endif



#ifdef	__cplusplus
}
#endif

#endif	/* L_HEARTBEAT_RING_H */

