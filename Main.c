#include <stdio.h>
#include <string.h>
#include <system.h>
#include <assert.h>
#include "io.h"
#include "nios2_ctrl_reg_macros.h"
#include "altera_up_avalon_rs232.h"
#include "sd_card.h"
#include "scanner.h"
#include "UART.h"
#include "timer.h"
#include "Server.h"

alt_up_ps2_dev *ps2;
KB_CODE_TYPE decode_mode;
char ascii;
alt_u8 buf[12];
unsigned char barcode[12];
unsigned char buffer[50];
unsigned char message[17];
unsigned char servermessage[16];
unsigned char ID[2];
alt_up_rs232_dev *uart;
unsigned char data;
unsigned char parity;
float start_time;
float current_time;
unsigned char checksumChar;
int count;
_Bool ready_flag;
int packetBuffer[10];
int packetCount;
int packetHead;
int packetTail;
char packetCountC[1];
int currentID;


int main()
{
 item list[TOTAL_ITEMS];

 initTimer();
 initScanner();
 initUART();
 clearUART();
 initSD();
 int i = 0;
 packetCount = 1;
 packetHead = 0;
 packetTail = 0;
 ready_flag = 0;

 if (getItems(list) == -1) printf("Error in reading SD card.\n");
 else printf("ITEMS READ FROM SD CARD\n");

 while(1){

	 alt_up_ps2_clear_fifo(ps2);
	 memset(buffer,0,sizeof(buffer));
	 memset(barcode,0,sizeof(barcode));

	 printf("\n--------------------\nSCAN, count = %i\n",count);

	 while ((count == 0) || (buffer[count-1] != '\0')){
		 recieveMessage();
	 }

	 count=0;

	 printf("BUFFER: %s\n",buffer);
	 printf("BUFFER LENGTH: %i\n",strlen(buffer));


	 if ((strlen(buffer) == 2) || (strlen(buffer) == 1)){
		 //ID code
		 for (i=0;i<strlen(buffer);i++){
			 ID[i] = buffer[i];
		 }
		 ID[strlen(buffer)] = '\0';
		 ready_flag = 1;
		 currentID = atoi(ID);
		 printf("ID: %i\n",currentID);
	 }
	 else if (strlen(buffer) == 12){
		 if (ready_flag == 1){

			 for (i=0; i<12; i++){
				 barcode[i] = buffer[i];
			 }

			 checksumChar = checksum(barcode);

			 barcode[12] = '\0';

			 item result = scan(barcode,list);

			 if (result.upc[0] == '\0'){
				 printf("BARCODE: %s\n",barcode);
				 printf("ITEM NOT FOUND: ADD ITEM!\n");
			 }
			 else{
				 printf("BARCODE: %s\n",barcode);
			 	 printf("ITEM FOUND: %s\n",result.upc);
			 	 printf("ITEM NAME: %s\n",result.name);
			 	 printf("PRICE: %.2f\n",result.price);
			 	 printf("STOCK: %d\n",result.stock);
			 	 if (result.stock > 0) {
			 		 //ID + SIZE + TYPE + PC + UPC + CS
			 		 message[0] = currentID;//ID
			 		 message[1] = 15;//SIZE
			 		 message[2] = "U";
					 message[3] = packetCount;
			 		 for (i=4; i<16; i++){
			 		 	 message[i] = barcode[i-4];
			 		 }
			 		 message[16] = checksumChar;
			 		 message[17] = '\0';
			 		 printf("MESSAGE: %s\n",message);
			 		 sendMessage(message);

			 		 servermessage[0] = 'B';
			 		 servermessage[1] = ',';
			 		 for (i=0; i<12; i++){
			 			 servermessage[i+2] = barcode[i];
			 		 }
			 		 servermessage[14] = ',';
			 		 servermessage[15] = '1';
			 		 printf("server message: %s\n",servermessage);
			 		 sendServerMessage(servermessage);

			 		 if (packetCount < 10) packetCount ++;
			 		 else packetCount = 1;
			 		 packetBuffer[packetCount] = packetCount;
			 		 packetTail ++;

			 		 int newstock = decreaseStockMemory(list,result);
			 		 printf("UPDATED STOCK: %d\n",newstock);
			 		 updateDatabase(list);
			 	 }
			 	 else {
			 		 printf("STOCK EMPTY - BARCODE NOT SENT TO APP\n");
			 	  	 printf("INCREASING STOCK\n");
			 	  	 int newstock = increaseStockMemory(list,result);
			 	  	 printf("UPDATED STOCK: %d\n",newstock);
			 	  	 updateDatabase(list);
			 	 }
			 }
		 }
	 }
 }
}
//------------------TIME CODE-------------------------------
//while (getTime() - start_time < 0.3);
//printf("current_time: %f, start_time: %f, count: %d, barcode: %s\n",current_time,start_time,count,barcode);
//strcat(barcode,buffer);
//-----------------------------------------------------------
