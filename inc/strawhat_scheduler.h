#ifndef STRAWHAT_SCHEDULER_H
#define STRAWHAT_SCHEDULER_H

#include "wlf_sched.h"
#include "strawhat_process.h"

/* Bit Positions of Key Flags */
#define BIT_READY       15
#define BIT_RUNNING     14
#define BIT_TERMINATED  13
#define BIT_CRITICAL    12
#define BIT_HIGH        11
#define BITS_EXIT_CODE  8

/* Type Equivalencies for the WLF Library */
typedef Wlf_schedule_s Scheduler_s;
typedef Wlf_queue_s Scheduler_queue_s;
typedef Wlf_process_s Scheduler_process_s;
typedef Wlf_create_data_s Scheduler_create_data_s;

/* Function Pointers for using the WLF Library */
typedef struct strawhat_scheduler_functions {
  Scheduler_s * (* scheduler_initialize) ();
  Scheduler_process_s * (* scheduler_create) (Scheduler_create_data_s *data);
  int (* scheduler_enqueue) (Scheduler_s *schedule, Scheduler_process_s *process);
  int (* scheduler_get_count) (Scheduler_queue_s *queue);
  Scheduler_process_s * (* scheduler_select) (Scheduler_s *schedule);
  int (* scheduler_suspend) (Scheduler_s *schedule, pid_t pid);
  int (* scheduler_resume) (Scheduler_s *schedule, pid_t pid);
  int (* scheduler_promote) (Scheduler_s *schedule);
  int (* scheduler_exited) (Scheduler_s *schedule, Scheduler_process_s *process, int exit_code);
  int (* scheduler_killed) (Scheduler_s *schedule, pid_t pid, int exit_code);
  int (* scheduler_reap) (Scheduler_s *schedule, pid_t pid);
  int (* scheduler_get_ec) (Scheduler_process_s *process);
  void (* scheduler_cleanup) (Scheduler_s *schedule);
} Scheduler_fns_s;

extern Scheduler_fns_s sched;

/* Prototypes for the StrawHat Scheduler Initialization */
void strawhat_scheduler_initialize();
Scheduler_create_data_s *strawhat_scheduler_initialize_data(Process_data_s *pdata);

#endif
