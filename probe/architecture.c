#include "os.h"
#include <stddef.h>

#include "architecture.h"

#include "simulation_parameters.h"


PREADYQUEUE tasks_queue = NULL;
PSCHEDULER single_scheduler = NULL, second_scheduler = NULL;
int init_architecture()
{
    Readyqueue_create(&tasks_queue);
    Scheduler_create(&single_scheduler,SCHEDULING_EARLIEST_DEADLINE_FIRST,tasks_queue,1);
    Scheduler_create(&second_scheduler,SCHEDULING_EARLIEST_DEADLINE_FIRST,tasks_queue,1);
    return 0;
}
