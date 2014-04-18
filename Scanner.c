#include "scanner.h"
#include "timer.h"

extern alt_up_ps2_dev *ps2;
extern KB_CODE_TYPE decode_mode;
extern char ascii;
extern alt_u8 buf[12];
extern int count=0;
extern unsigned char buffer[50];
extern float start_time;

static void keyboard_ISR( void *c, alt_u32 id )
{
	decode_scancode(ps2,&decode_mode, buf, &ascii);
	if (decode_mode == 1){
		buffer[count] = ascii;
		count++;
	}
	if ((decode_mode == 2) && (buf[0] == 'Z')){
		buffer[count] = '\0';
		count++;
	}
}

void initScanner(){
	//for scanner
	 ps2 = alt_up_ps2_open_dev(KEYBOARD_NAME);
	 alt_up_ps2_init(ps2);
	 alt_up_ps2_clear_fifo(ps2);
	 void* keyboard_control_register_ptr = (void*) (KEYBOARD_BASE + 4);
	 alt_irq_register(KEYBOARD_IRQ, keyboard_control_register_ptr, keyboard_ISR);
	 alt_up_ps2_enable_read_interrupt(ps2);
}

unsigned char checksum(unsigned char barcode []){
	unsigned char sum=0;
	int i=0;
	for (i = 0; i < 12; i++){
		sum = barcode[i] + sum;
	}
	return sum;
}
