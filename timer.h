#include <stdio.h>
#include <system.h>
#include <sys/alt_irq.h>
#include "nios2_ctrl_reg_macros.h"
#include "io.h"
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"


void initTimer();
float getTime();
