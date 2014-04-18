#include "sd_card.h"

char database_name[12];
int version = 0;

void initSD(){
	int connected = 0;
	alt_up_sd_card_dev *device_reference = NULL;
	device_reference = alt_up_sd_card_open_dev(SD_CARD_NAME);
	if (device_reference != NULL) {

			if ((connected == 0) && (alt_up_sd_card_is_Present())) {
				//printf("Card connected. \n");
				//printf("fat16: %d",alt_up_sd_card_is_FAT16());
				if (alt_up_sd_card_is_FAT16());// printf("FAT16 file system detected. \n");
				else printf("Unknown file system. \n");
				connected = 1;
			} else if ((connected == 1) && (alt_up_sd_card_is_Present()
					== false)) {
				printf("Card disconnected. \n");
				connected = 0;
			}
	}
	findDatabaseVersion();
	setDatabase(0);
}

void findDatabaseVersion(){
	if(alt_up_sd_card_find_first("",database_name)==0){
		while(alt_up_sd_card_find_next(database_name)==0){
				//printf("%s \n",database_name);
				version++;
		}
	}
	else if (alt_up_sd_card_find_first("",database_name)== -1)
		printf("No files. \n");
}

void setDatabase(int rewrite){
	if (rewrite == 1)
		version++;
	//printf("DATABASE: %s\n",database_name);
	if (version < 10)
		database_name[6] = (char)(((int)'0')+version);
	else if (version >= 10 && version <100){
		database_name[5] = (char)(((int)'0')+version/10);
		database_name[6] = (char)(((int)'0')+version%10);
	}
	else{
		database_name[4] = (char)(((int)'0')+version/100);
		database_name[5] = (char)(((int)'0')+(version/10)%10);
		database_name[6] = (char)(((int)'0')+version%10);
	}
	printf("Database: %s\n",database_name);
}

int getItems(item *list){
	short int handler = alt_up_sd_card_fopen(database_name, false);
	if (handler == -1) return -1;
	int i=0 ,j=0; // j = index of items
	char word = alt_up_sd_card_read(handler);
	unsigned char tmp_string[10];

	while (j < TOTAL_ITEMS){
		//printf("j=%d\n", j);
		if (j == 0){
			// Get rid of UTF-8 BOM
			if (strcmp(database_name,"data000.txt") == 0){
				word = alt_up_sd_card_read(handler);
				word = alt_up_sd_card_read(handler);
				word = alt_up_sd_card_read(handler);
			}
		}
		// UPC
		for (i=0;i<UPC_NUM_CHARACTER;i++){
			//printf("word: %c\n",word);
			list[j].upc[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		list[j].upc[i] = '\0';
		//printf("upc: %s\n",list[j].upc);
		word = alt_up_sd_card_read(handler); // Pass the Comma

		// Name - up to quotation mark '"'
		i=0;
		list[j].name[i] = word;
		i++;
		word = alt_up_sd_card_read(handler);
		for (;(int)word != 34;i++){
			list[j].name[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		list[j].name[i++] = word;
		list[j].name[i] = '\0';
		word = alt_up_sd_card_read(handler);
		word = alt_up_sd_card_read(handler);
		//printf("name: %s\n",list[j].name);

/*
		// Description
		i=0;
		list[j].description[i] = word;
		i++;
		word = alt_up_sd_card_read(handler);
		for (;(int)word != 34;i++){
			list[j].description[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		list[j].description[i++] = word;
		list[j].description[i] = '\0';
		word = alt_up_sd_card_read(handler);
		word = alt_up_sd_card_read(handler);
*/
		// Price
		for (i=0; (int)word != 44 ; i++){
			tmp_string[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		tmp_string[i] = '\0';
		//printf("price_string: %s\n",price_string);
		list[j].price = atof(tmp_string);
		word = alt_up_sd_card_read(handler);
		//printf("price: %.2f\n",list[j].price);
/*
		// Category
		i=0;
		list[j].category[i] = word;
		i++;
		word = alt_up_sd_card_read(handler);
		for (;(int)word != 34;i++){
			list[j].category[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		list[j].category[i++] = word;
		list[j].category[i] = '\0';
		word = alt_up_sd_card_read(handler);
		word = alt_up_sd_card_read(handler);
*/
		// Stock
		for (i=0;(int)word != 10;i++){
			//printf("word: %c\n",word);
			tmp_string[i] = word;
			word = alt_up_sd_card_read(handler);
		}
		tmp_string[i] = '\0';
		list[j].stock = atoi(tmp_string);
		//printf("stock: %d\n",list[j].stock);

		word = alt_up_sd_card_read(handler);
		// Next item
		j++;
	}

	alt_up_sd_card_fclose(handler);

	return 0;
}

item scan(unsigned char upc[],item list[]){
	item result;
	result.upc[0] = NULL;
	int i = 0;
	while (i < TOTAL_ITEMS){
		if (strcmp(list[i].upc,upc) == 0){
			result = list[i];
			break;
		}
		i++;
	}

	return result;
}

int decreaseStockMemory(item list[], item to_decrease){
	int i = 0,j;
	unsigned char stock_string[4] = {'\0'};
	while(i < TOTAL_ITEMS){
		//printf("i: %d\n",i);
		if (strcmp(list[i].upc,to_decrease.upc) == 0){
			if (list[i].stock > 0)
				list[i].stock--;
			break;
		}
		i++;
	}
	return list[i].stock;
}

int increaseStockMemory(item list[], item to_increase){
	int i = 0,j;
	unsigned char stock_string[4] = {'\0'};
	while(i < TOTAL_ITEMS){
		//printf("i: %d\n",i);
		if (strcmp(list[i].upc,to_increase.upc) == 0){
			if (list[i].stock == 0)
				list[i].stock++;
			break;
		}
		i++;
	}
	return list[i].stock;
}

void updateDatabase(item list[]){
		setDatabase(1);
		short int handler = alt_up_sd_card_fopen(database_name, true);
		unsigned char tmp[5] = {'\0'};
		int i,j;
		//printf("Updating to %s...\n",database_name);
		if (handler == -1){
			printf("Error occurred when updating database.");
		}

		// Write data into the file
		for (i=0;i<TOTAL_ITEMS;i++){
			for (j=0;j<UPC_NUM_CHARACTER;j++){
				alt_up_sd_card_write(handler, list[i].upc[j]);
			}
			alt_up_sd_card_write(handler, ',');

			for (j=0;list[i].name[j] != '\0';j++){
				alt_up_sd_card_write(handler, list[i].name[j]);
			}
			alt_up_sd_card_write(handler, ',');

			sprintf( tmp, "%.2f", list[i].price);
			for (j=0;tmp[j] != '\0';j++){
				alt_up_sd_card_write(handler, tmp[j]);
			}
			alt_up_sd_card_write(handler, ',');

			sprintf( tmp, "%d", list[i].stock);
			for (j=0;tmp[j] != '\0';j++){
				alt_up_sd_card_write(handler, tmp[j]);
			}
			alt_up_sd_card_write(handler, 13); // CR
			alt_up_sd_card_write(handler, 10); // LF
		}
		alt_up_sd_card_fclose(handler);

		//printf("Database updated.\n");
}
