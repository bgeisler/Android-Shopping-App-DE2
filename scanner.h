#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include <string.h>
#include <sys/alt_irq.h>
#include "altera_up_ps2_keyboard.h"
#include "altera_up_avalon_ps2.h"

static void keyboard_ISR( void *, alt_u32);
void initScanner(void);
unsigned char checksum(unsigned char []);
