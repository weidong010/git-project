/**    
       * Test_Main.c shows how to use Heartbeat API
       * used as multi-threaded detection applications 
       * made by Weidong Wang on Aug 13, 2014  */
#define NUM_THREADS 5
#define NUM_HEARTBEAT 1000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "L_Heartbeat_Interface.h"

/**
       * Test_Main.c shows how to use Heartbeat API
       * used as multi-threaded detection applications */
/**
       * Task PrintHello(), function is used in multi-threaded;
       * @ */
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
void *PrintHello(void* p_matrix){
    
   heartbeat_profile_t heart;
   pthread_t tid;
   int i;
   int rc;
/**
       * Heartbeat initialized  
       * @1:  heartbeat_profile_t heart 
       * @2:                                */   
/**
       * Heartbeat initialization;
       * @ */  
   heartbeat_sequence* pt = NULL;

   pt=(heartbeat_sequence*)p_matrix;

   rc = L_Heartbeat_Init(&heart,pt,1000,1000);

   printf("Heartbeats Done\n");
   
   if(rc != 0)
   printf("Error allocating heartbeat data %d\n", rc);

   for (i=0;i<1000;i++){     

    L_Heartbeat_Generate(&heart,pt,i,i,2); 
    
   }
 
      L_Heartbeat_End(&heart);
   printf("Heartbeats Done\n");
         pthread_exit(0);
}

int main(int argc, char** argv) {
  pthread_t threads[NUM_THREADS];
  int pth_rc;
  int pth_i;
  int i;
  void *status=NULL;
  heartbeat_sequence arg[NUM_THREADS];
  for(pth_i=0;pth_i<NUM_THREADS;pth_i++){
  arg[pth_i].p = L_Heartbeat_Alloc_Global_Matrix(NUM_HEARTBEAT);
  arg[pth_i].amount=pth_i;
  printf("arg[pth_i].p=%d\n",arg[pth_i].p);
  }

  for (pth_i=0;pth_i<5;pth_i++){
       printf("In main: creating thread %d\n",pth_i);
       pth_rc=pthread_create(&threads[pth_i],NULL, PrintHello,(void*)&arg[pth_i]);
       usleep(1000);
       printf("thread %d=%u\n",pth_i,(unsigned int)threads[pth_i]);         
       if(pth_rc){
       printf("ERROR: return code from pthread_creat() is %d\n",pth_rc);
       exit(-1);
       }
  }
    pthread_cancel(threads[2]);
  //  L_Hearteat_Monitor_A(arg,NUM_THREADS, NUM_HEARTBEAT,2);
    
   for (pth_i=0;pth_i<5;pth_i++){
       pthread_join(threads[pth_i],NULL);
       printf("this is me");  
   }
  
   for (pth_i=0;pth_i<5;pth_i++){    
   printf("arg[%d].p.heartbeat_rate=%lf\n",pth_i,arg[pth_i].p[10].heartbeat_rate);      
   }
  
   for (pth_i=0;pth_i<5;pth_i++){     
   printf("arg[%d].amount=%ld, hs->last=%lf\n",pth_i,arg[pth_i].amount, arg[pth_i].last->heartbeat_rate);       
   }
  
  L_Hearteat_Monitor_A(arg,NUM_THREADS, NUM_HEARTBEAT,2);
   
  printf("all done\n");
   return 0;
}

