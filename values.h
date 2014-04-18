#ifndef VALUES_INCLUDE_H
#define VALUES_INCLUDE_H

#define TOTAL_ITEMS 278
#define UPC_NUM_CHARACTER 12

typedef struct _item_{
	unsigned char upc[UPC_NUM_CHARACTER+1];
	unsigned char name[100];
	double price;
	int stock;
} item;
#endif


