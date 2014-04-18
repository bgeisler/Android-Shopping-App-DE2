#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include <string.h>
#include <altera_up_sd_card_avalon_interface.h>
#include <time.h>

#include "values.h"

void initSD(void);
void findDatabaseVersion(void);
void setDatabase(int);
int getItems(item[]);
item scan(unsigned char[], item[]);
int decreaseStockMemory(item[],item);
int increaseStockMemory(item[],item);
void updateDatabase(item[]);
