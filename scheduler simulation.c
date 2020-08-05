
/**********************************************************************/
/*                                                                    */
/* Program Name: Scheduler Simulation Project                         */
/* Author:       Caleb Patterson                                      */
/* Installation: Pensacola Christian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: March 14, 2019                                       */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* I pledge  the C language  statements in  this  program are  my own */
/* original  work,  and none  of the  C language  statements in  this */
/* program were copied  from any one else,  unless I was specifically */
/* authorized to do so by my CS326 instructor.                        */
/*                                                                    */
/*                                                                    */
/*                           Signed: ________________________________ */
/*                                             (signature)            */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* This program simulates the environment in Unix where new processes */
/* are continually arriving, existing processes are vying for the CPU */
/* , processes are using there given quantum or blocking because of   */
/* I/0 operations, and processes are terminating when their work is   */
/* finished                                                           */
/*                                                                    */
/**********************************************************************/
#include <stdio.h>    /* printf                                       */
#include <stdlib.h>   /* random                                       */

/**********************************************************************/
/*                        Symbolic Constants                          */
/**********************************************************************/
#define INITIALIZE_AMOUNT  5    /* Amount of processes to be          */
                                /* initialized                        */
#define MAX_PROCESSES      10   /* Maximum number of processes in     */
                                /* table                              */
#define TRUE               1    /* Value of true                      */
#define FALSE              0    /* Value of false                     */
#define MAX_PID            100  /* Maximum PID number allowed in      */
                                /* table                              */
#define RUNNING            'N'  /* Running process                    */
#define READY              'R'  /* Ready process                      */
#define BLOCKED            'B'  /* Blocked process                    */

/**********************************************************************/
/*                        Program Structures                          */
/**********************************************************************/
/* Computer processes                                                 */
struct process
{
   int  blk_time,                        /* How many ticks till       */
        cpu_used,                        /* process blocked           */
                                         /* Time process ran on CPU   */
        max_time,                        /* Max time to run on CPU    */
        pid,                             /* Process ID                */
        pri,                             /* Process priority          */
        quantum_used,                    /* Quantum used by process   */
        wait_tks;                        /* Ticks ready process has   */
                                         /* been waiting              */
   char state;                           /* Process state             */
}; 
typedef struct process PROCESS;

/**********************************************************************/
/*                       Function Prototypes                          */
/**********************************************************************/
void create_process (PROCESS process_table[], int *p_number_processes, 
                     int *p_next_pid);
   /* Create and place a new process into the process table           */
void initialize_table (PROCESS process_table[], int *p_number_processes, 
                       int *p_next_pid);   
   /* Initialize processes into the process table                     */
void  reschedule_pri(PROCESS process_table[], int array, 
                     int number_processes);
   /* Reschedule the priority of a process                            */
void check_blocked(PROCESS process_table[], int *p_ready);
   /* Check to see if a process has become unblocked                  */
void print_table(PROCESS process_table[], int next_pid, 
                 int number_processes);
   /* Print the contents of the process table                         */
void add_process(PROCESS process_table[], int *p_next_pid, 
                 int *p_number_processes, int *p_ready);
   /* Check to see if a new process is added to the process table     */

void order_processes( PROCESS process_table[], int number_processes);
   /* Sort the process table                                          */
void delete_process (PROCESS process_table[], int index);
   /* Delete a process                                                */
void swap_processes(PROCESS process_table[], int index, int index2);
   /* Swap two processes                                              */
int number_negative(PROCESS process_table[]);
   /* Count the number of negative priority processes                 */
void update_wait_ticks(PROCESS process_table[]);
   /* Update the wait ticks of ready processes                        */
void process_tick(PROCESS process_table[], int number_processes);
   /* Process the ticks of the running process                        */
 void terminate_process(PROCESS process_table[], int next_pid, 
                        int *p_number_processes, int *p_schedule, 
                        int *p_ready);
   /* Terminate the running process if it has reached max time        */
 void preempt_process(PROCESS process_table[],int next_pid, 
                      int number_processes, int *p_schedule, 
                      int *p_ready);
   /* Preempt the running process if it has reached block time or     */
   /* all of its quantum                                              */
 void schedule_process(PROCESS process_table[],int next_pid, 
                       int number_processes, int *p_none_ready, 
                       int *p_schedule  );
 /* Schedule the next process if necessary                            */ 
 void insert_in_queue(PROCESS process_table[], int index, 
                      int number_processes);
 /* Insert process into proper queue                                  */

/**********************************************************************/
/*                          Main Function                             */
/**********************************************************************/
int main()
{
   PROCESS process_table[MAX_PROCESSES]; /* Process table             */
   
   int     next_pid         = 1,         /* Counts through the        */
                                         /* process ids               */
           none_ready       = FALSE,     /* Flag if no processes are  */
                                         /* ready                     */
           number_processes = 0,         /* Number of processes in    */
                                         /* schedule                  */
           ready            = FALSE,     /* Flag if process has since */
                                         /* become ready              */
           schedule         = TRUE;      /* Flag that decides to      */
                                         /* schedule                  */

   /*Initalize the process table and then print the contents          */
   initialize_table(process_table, &number_processes, &next_pid);
   printf("\n\n BEFORE SCHEDULING CPU:  ");
   print_table(process_table, next_pid, number_processes);
  
   /* Loop processing clock ticks until max process ID is reached     */
   while (next_pid <= MAX_PID)
   {
       /* Print the process that has now become ready                 */
       if (ready && none_ready)
       {
          printf("\n\n BEFORE SCHEDULING CPU:  ");
          print_table(process_table, next_pid, number_processes);
          schedule = TRUE;
       }
       
       /* Update the running processes quantum and CPU used           */
       process_tick(process_table, number_processes);
       
       /* Terminate the running process if it has reached its maximum */
       /* CPU time                                                    */
       terminate_process(process_table, next_pid, &number_processes,  
                         &schedule, &ready);      
       
       /* Preempt the running process if it is blocked or has used    */
       /* all of its quantum                                          */
       preempt_process(process_table, next_pid, number_processes,  
                       &schedule, &ready);
       
       /* Schedule the next process if necessary                      */
       if (schedule)
          schedule_process(process_table, next_pid, number_processes, 
                           &none_ready, &schedule );
       
       /* Make ready any blocked processes that have become unblocked */
       check_blocked(process_table, &ready);

       /* Place any newly arrived processes into the process table    */
       add_process(process_table, &next_pid, &number_processes, &ready);
       
       /* Update the wait ticks of ready processes                    */
       update_wait_ticks(process_table);
    }   
    int t;
    scanf_s("%d", &t);
    return 0;
}

/**********************************************************************/
/*                 Initialize the process table                       */
/**********************************************************************/
void initialize_table (PROCESS process_table[], int *p_number_processes,
                       int *p_next_pid)
{
   int array_index;        /* Index of every process                  */
    
   /* Fill the array with empty processes                             */
   for (array_index = 0; array_index < MAX_PROCESSES; array_index++)
      process_table[array_index].pid      =  0;
   
   /*Initialize the array with the first processes                    */
   for (array_index = 0; array_index < INITIALIZE_AMOUNT; array_index++)
      create_process (process_table, p_number_processes, p_next_pid);

   return;
}

/**********************************************************************/
/*                        Create a process                            */
/**********************************************************************/
void create_process (PROCESS process_table[], int *p_number_processes,
                     int *p_next_pid)
{
   int index; /* Index to the process that is created                 */
   
   /* Insert the created process into the first available spot        */
   for(index=0; index< MAX_PROCESSES; index++)
   {
      if (process_table[index].pid == 0)
      {   
         process_table[index].pid            = *p_next_pid;
         process_table[index].cpu_used       = 0;
         process_table[index].max_time       = (rand()%18)+1;
         process_table[index].state          = READY;
         process_table[index].pri            = 0;
         process_table[index].quantum_used   = 0;
         process_table[index].blk_time       = (rand()%6)+1;
         process_table[index].wait_tks       = 0;
         (*p_next_pid)++;
         (*p_number_processes)++;
         index = MAX_PROCESSES;
      }
   }
   return;
 }

/**********************************************************************/
/*                      Reschedule Priority                           */
/**********************************************************************/
void reschedule_pri(PROCESS process_table[], int index, 
                    int number_processes)
{
   /* Block or ready the process and reasign it's priority            */
   if(process_table[index].quantum_used < 6)
   {
      process_table[index].state = BLOCKED;
      process_table[index].pri   = (int) ((((float) 
         abs(process_table[index].pri) + process_table[index].quantum_used)
         / 2.0f) + .5f);
     process_table[index].pri = (process_table[index].pri * -1);
   }
   else
   {   process_table[index].state = READY;
       process_table[index].pri = (int) ((((float) 
          abs(process_table[index].pri) + process_table[index].quantum_used)
          / 2.0f) + .5f);
   }
   
   /* Resest quantum used to 0 and properly swap into correct         */
   /* within the queue                                                */
   process_table[index].quantum_used = 0;
   insert_in_queue(process_table, index, number_processes);   
   
   return;
}

/**********************************************************************/
/*                Check to unblock blocked processes                  */
/**********************************************************************/
void check_blocked(PROCESS process_table[], int *p_ready)
{
   int index; /* Index to of every process                            */
   
   /*Loop processing blocked processes to check if they should be     */
   /*unblocked                                                        */
   for (index = 0; index <= 9; index++)
   {
      if (process_table[index].state == BLOCKED)
      {
        if(rand()%20==0)
        {
           process_table[index].state = READY;
           *p_ready = TRUE;
        }
      }
   }  
   return;
}
/**********************************************************************/
/*                       Print the process table                      */
/**********************************************************************/
void print_table(PROCESS process_table[], int next_pid, 
                 int number_processes)
{
   int index; /*index to every process to be printed                  */
   
   printf("Next PID = %2d,   Number of Processes = %2d", next_pid, 
                                                      number_processes);
   printf("\n PID   CPU Used   MAX Time   STATE   PRI   QUANTUM USED   ");
   printf("BLK TIME   WAIT TKS");
   for (index=0; index < number_processes; index++)
   {  
      printf("\n %3d", process_table[index].pid);
      printf("%8d", process_table[index].cpu_used);
      printf("%11d", process_table[index].max_time);
      printf("%9c", process_table[index].state);
      printf("%10d", process_table[index].pri);
      printf("%8d", process_table[index].quantum_used);
      printf("%13d", process_table[index].blk_time);
      printf("%12d", process_table[index].wait_tks);	  
   }
   return;
}

/**********************************************************************/
/*              Check to see if a new process is added                */
/**********************************************************************/
void add_process(PROCESS process_table[], int *p_next_pid, 
                 int *p_number_processes, int *p_ready)
{
   /*Check to see if a new process will be created                    */
   if (rand()%5==0 && *p_number_processes < MAX_PROCESSES)
   {
      create_process(process_table, p_number_processes, p_next_pid);
      *p_ready = TRUE;
      order_processes(process_table, *p_number_processes);
   }
   
   return;
}

/**********************************************************************/
/*                    Sort the process table                          */
/**********************************************************************/
void order_processes(PROCESS process_table[], int number_processes)
{
   int counter, /*counts through the outer loop                       */
       index;   /*index of every process                              */
   
   /* Sort the processes into ascending order                         */
   for(counter=1; counter<number_processes; counter++)
      for(index=0; index < (number_processes - counter); index++)
         if(process_table[index].pri > process_table[index + 1].pri)
            swap_processes(process_table, index, index+1);
        
   /* Sort the negative priority processes into descending order      */
   for(counter=1; counter<number_negative(process_table); counter++)
      for(index=0; index < (number_negative(process_table) - counter);
                                                                index++)
         if(process_table[index].pri < process_table[index + 1].pri)
            swap_processes(process_table, index, index+1);

   return;
}

/**********************************************************************/
/*                       Delete the process                           */
/**********************************************************************/
void delete_process (PROCESS process_table[], int index)
{
   process_table[index].pid = 0;
   process_table[index].pri = 99;
   
   /* Move the deleted process to the end of the array                */
   for(; index< MAX_PROCESSES - 1; index++)
   {   
      if(process_table[index].pri > process_table[index+1].pri)
      {
         swap_processes(process_table, index, index + 1);
      }
   }
   return;
}
/**********************************************************************/
/*                      Swap processes                                */
/**********************************************************************/
void swap_processes(PROCESS process_table[], int index, int index2)
{
   PROCESS temp; /* Temporarily holds a process to be swapped         */
   
   /* Swap two processes                                              */
   temp = process_table[index];
   process_table[index] = process_table[index2];
   process_table[index2] = temp;
   
   return;
}

/**********************************************************************/
/*          Count how many negative priorities there are              */
/**********************************************************************/
int number_negative(PROCESS process_table[])  
{
   int index,             /* Indexs of every process                  */
       number_negative=0; /* The number of negative priorities        */
    
   /*Loop processing through array counting negative priorities       */
   for(index=0; index < MAX_PROCESSES; index++)
      if (process_table[index].pri < 0 && process_table[index].pid != 0)
         number_negative++;
   
   return number_negative;
}

/**********************************************************************/
/*                Update ready processes wait ticks                   */
/**********************************************************************/
void update_wait_ticks(PROCESS process_table[])
{
   int index; /*index to reference array elements*/
   
   /*Loop updating the wait ticks of every ready process*/
   for(index=0; index < MAX_PROCESSES; index++)
      if (process_table[index].state == READY)
         process_table[index].wait_tks++;
 
   return;
}

/**********************************************************************/
/*               Update ticks for runnning process                    */
/**********************************************************************/
void process_tick(PROCESS process_table[], int number_processes)
{
   int index; /* Index to every process                               */

   /* Loop processing the ticks of running process                    */
   for(index=0; index < number_processes; index++)
      if (process_table[index].state == RUNNING)
      {
         process_table[index].quantum_used++;
         process_table[index].cpu_used++;
      }
   
   return;
}

/**********************************************************************/
/*         Terminate processes that have reached max time             */
/**********************************************************************/
void terminate_process(PROCESS process_table[], int next_pid, 
                       int *p_number_processes, int *p_schedule, 
                       int *p_ready)
{
   int index; /* Index of every process                               */
       
   /* Loop processing processes to check if they have reached max     */
   /* time                                                            */
   for(index=0; index < *p_number_processes; index++)
      if (process_table[index].cpu_used == process_table[index].max_time)
      {
         printf("\n\n BEFORE SCHEDULING CPU:  ");
         print_table(process_table, next_pid, *p_number_processes);
         delete_process(process_table, index);
         (*p_number_processes)--;
         *p_schedule = TRUE;
         *p_ready    = FALSE;
      }
   return;
}

/**********************************************************************/
/*           Preempt a process that has reached block time            */
/**********************************************************************/
void preempt_process(PROCESS process_table[],int next_pid, 
                     int number_processes, int *p_schedule, int *p_ready)
{
   int index; /* Index of every process                               */
   
   /* Loop processing processes to check if they have block time      */
   for(index=0; index < number_processes; index++)
      if (process_table[index].quantum_used == 
                                          process_table[index].blk_time)
      {
         printf("\n\n BEFORE SCHEDULING CPU:  ");
         print_table(process_table, next_pid, number_processes);
         reschedule_pri(process_table, index, number_processes);
         order_processes(process_table, number_processes);
         *p_schedule = TRUE;
         *p_ready    = FALSE;
      }
   
   return;
}

/**********************************************************************/
/*                Schedule the next ready process                     */
/**********************************************************************/
void schedule_process(PROCESS process_table[],int next_pid, 
                      int number_processes, int *p_none_ready, 
                      int *p_schedule )
{
   int index; /* Index to every process                               */

   /* Loop processing processes trying to schedule a ready process to */ 
   /* a running process                                               */
   for(index=0; index <= number_processes; index++)
      if (process_table[index].state == READY)
      {
         process_table[index].state = RUNNING;
         index = 11;
         *p_none_ready = FALSE;
      }
      else
         *p_none_ready = TRUE;
      *p_schedule = FALSE;
   
   /* Print the contents of the process table after scheduling        */ 
   printf("\n\n AFTER SCHEDULING CPU:  ");
   print_table(process_table, next_pid, number_processes);
      
   return;
}

/**********************************************************************/
/*           Insert the process into the correct queue                */
/**********************************************************************/
void insert_in_queue(PROCESS process_table[], int index, 
                     int number_processes)
{
   int  index2; /* Second index of every process                      */
   
   if (process_table[index].pri < 0)
   {  
      /* If process is negative place it into the appropriate queue   */
      for (index2 = 0; index2 < number_processes; index2++)
         if( process_table[index].pri > process_table[index2].pri || 
            process_table[index2].pri == 0)
         {   
            swap_processes(process_table, index, index2);
            index2 = MAX_PROCESSES;
         }
   }
   
   /* If process is positive place it into the appropriate queue      */
   if (process_table[index].pri > 0)
   {
      for (index2 = 0; index2 < number_processes; index2++)
         if( process_table[index].pri < process_table[index2].pri|| 
            index2 == MAX_PROCESSES)
         {
         swap_processes(process_table, index, index2);
         index2 = MAX_PROCESSES;
         }
   }
   return;
}