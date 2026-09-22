#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
/* StrawHat VM Includes */
#include "strawhat.h"
#include "wlf_sched.h"
#include "strawhat_scheduler.h"
#include "strawhat_support.h"
#include "strawhat_process.h"
#include "strawhat_printing.h"
#include "strawhat_cs.h"

Scheduler_fns_s sched = {0};

void strawhat_scheduler_initialize() {
  sched.scheduler_initialize = wlf_initialize;
  sched.scheduler_create = wlf_create;
  sched.scheduler_enqueue = wlf_enqueue;
  sched.scheduler_get_count = wlf_count;
  sched.scheduler_select = wlf_select;
#if FEATURE_SUSPEND  
  sched.scheduler_suspend = wlf_suspend;
  sched.scheduler_resume = wlf_resume;
#endif
#if FEATURE_MLFQ  
  sched.scheduler_promote = wlf_promote;
#endif  
  sched.scheduler_exited = wlf_exited;
  sched.scheduler_killed = wlf_killed;
#if FEATURE_REAP  
  sched.scheduler_reap = wlf_reap;
#endif  
  sched.scheduler_get_ec = wlf_get_ec;
  sched.scheduler_cleanup = wlf_cleanup;
}

Scheduler_create_data_s *strawhat_scheduler_initialize_data(Process_data_s *pdata) {
  Scheduler_create_data_s *sdata = calloc(1, sizeof(Scheduler_create_data_s));
  sdata->is_high = pdata->is_high;
  sdata->is_critical = pdata->is_critical;
  sdata->pid = pdata->pid;

  strncpy(sdata->original_cmd, pdata->input_orig, strlen(pdata->input_orig) + 1);
  return sdata;
}