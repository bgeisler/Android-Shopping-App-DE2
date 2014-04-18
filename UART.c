#include "UART.h"
#include "timer.h"

extern alt_up_rs232_dev *uart;
extern unsigned char data;
extern unsigned char parity;
extern int packetBuffer[10];
extern int packetCount;
extern int packetHead;
extern int packetTail;

void initUART(){
	uart = alt_up_rs232_open_dev(RS232_0_NAME);
}

void clearUART(){
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart)){
		alt_up_rs232_read_data(uart, &data, &parity);
	}
}

void sendMessage(unsigned char barcode[]){
	 int i;
	 for (i = 0; i < strlen(barcode); i++){
		alt_up_rs232_write_data(uart, barcode[i]);
	 }
}

_Bool ID_p = 0;
_Bool size_p =0;
_Bool type_p = 0;
_Bool pc_p = 0;
_Bool bool_p = 0;

unsigned char ID_r;
unsigned char num_r;
unsigned char type_r;
unsigned char pc_r;
unsigned char bool_r;
unsigned char IDmessage[10];

unsigned char ServerID;


void recieveMessage(){

	int i;

	if (alt_up_rs232_get_used_space_in_read_FIFO(uart) > 0){
		printf("recieving data\n");

		alt_up_rs232_read_data(uart, &data, &parity);
		printf("data: %d,%i,%c\n",data,data,data);
		if (ID_p == 0){
			ID_r = data;
			ID_p = 1;
			printf("ID: %i\n",ID_r);
		}
		else if(size_p == 0){
			num_r = data;
			size_p = 1;
			printf("SIZE: %i\n",num_r);
		}
		else if(type_p == 0){
			type_r = data;
			type_p = 1;
			printf("TYPE: %c\n",type_r);
			if (type_r == 'P'){
				ID_p = 0;
				size_p = 0;
				type_p = 0;
				printf("P RECIEVED\n");
				ServerID = ID_r;
				printf("serverID: %d\n",ServerID);
				float start_time = getTime();
				while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0){
					if (getTime()-start_time >= 60) break;
				}
				alt_up_rs232_read_data(uart, &data, &parity);
				printf("second data byte: %d !!!\n",data);
				printf("NEXT\n");
			}
			else if (type_r == 'I'){
				printf("ID %d,NUM %d,TYPE: %c\n",ID_r,num_r,type_r);
				ID_p = 0;
				size_p = 0;
				type_p = 0;
				IDmessage[0] = ID_r;
				IDmessage[1] = 4;
				IDmessage[2] = 'I';//type
				IDmessage[3] = packetCount;//this should be packetCount
				IDmessage[4] = ID_r;//'1';
				IDmessage[5] = 'C';//this should be checksum
				IDmessage[6] = '\0';
				sendMessage(IDmessage);
				printf("send ID: %d\n",IDmessage[4]);
				printf("sent message: %s\n",IDmessage);
				packetBuffer[packetCount] = packetCount;
				if (packetCount < 10) packetCount ++;
				else packetCount = 1;
				packetTail ++;
			}
		}
		else if ((type_r == 'S') && (pc_p == 0)){
			pc_r = data;
			pc_p = 1;
			packetBuffer[pc_r] = NULL;
			packetHead ++;
		}
		else if ((type_r == 'S') && (bool_p == 0)){
			bool_p = 1;
			bool_r = data;
			printf("ID %i,num %i, type %c, pc %c, bool %c",ID_r,num_r,type_r,pc_r,bool_r);
			ID_p = 0;
			size_p = 0;
			type_p = 0;
			pc_p = 0;
			bool_p = 0;
		}
	}
}
