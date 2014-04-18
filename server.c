#include "Server.h"
#include "UART.h"

extern alt_up_rs232_dev *uart;
extern unsigned char data;
extern unsigned char parity;
extern unsigned char ServerID;

void sendServerMessage(unsigned char messageS[]){

	int i;
	printf("enter send server message\n");
	//unsigned char message[] = "B,058716989744,1";
	//SIZE--wont need for option 4
	alt_up_rs232_write_data(uart, ServerID);
	alt_up_rs232_write_data(uart, (unsigned char) strlen(messageS));
	//message
	for (i = 0; i < strlen(messageS); i++) {
		printf("%c", messageS[i]);
	    alt_up_rs232_write_data(uart, messageS[i]);
	}
}

