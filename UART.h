#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include <string.h>
#include "altera_up_avalon_rs232.h"

void initUART(void);
void clearUART(void);
void sendMessage(unsigned char[]);
void recieveMessage(void);
