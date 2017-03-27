/*

 max7219.c
 to compile : gcc max7219.c -o max7219 -lwiringPi

*/
 
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>


// define our pins :

#define DATA        12 
#define CLOCK       14 
#define LOAD        21


// The Max7219 Registers :

#define DECODE_MODE   0x09                       
#define INTENSITY     0x0a                        
#define SCAN_LIMIT    0x0b 
#define SHUTDOWN      0x0c                        
#define DISPLAY_TEST  0x0f                         

#define DELAY_TIME 50 

unsigned char firework[64] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011000,
	0b00011000,
	0b00000000,
	0b00000000,
	0b00000000,

        0b00000000,
        0b00000000,
        0b00100100,
        0b00011000,
        0b00011000,
        0b00100100,
        0b00000000,
        0b00000000,

        0b00000000,
        0b00000000,
        0b00100100,
        0b00000000,
        0b00000000,
        0b00100100,
        0b00000000,
        0b00000000,

        0b00000000,
        0b01000010,
        0b00100100,
        0b00000000,
        0b00000000,
        0b00100100,
        0b01000010,
        0b00000000,

        0b00000000,
        0b01000010,
        0b00000000,
        0b00010000,
        0b00001000,
        0b00000000,
        0b01000010,
        0b00000000,

        0b10000001,
        0b01000010,
        0b00000000,
        0b00001000,
        0b00010000,
        0b00000000,
        0b01000010,
        0b10000001,

        0b10000001,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b10000001,

        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
};


static void Send16bits (unsigned short output) {
   
  signed char i;

  for (i=15; i>=0; i--) {
    digitalWrite(CLOCK, 0);  // set clock to 0
    
    if ((output >> i) & 0x01)   {
      digitalWrite(DATA, 1);          
    } else {
      digitalWrite(DATA, 0);  
    }
        
    digitalWrite(CLOCK, 1);  // set clock to 1
  }

}

static void MAX7219Send (unsigned char reg_number, unsigned char dataout) {
  digitalWrite(LOAD, 0);  // set LOAD 1 to start
  Send16bits((reg_number << 8) + dataout);   // send 16 bits ( reg number + dataout )
  digitalWrite(LOAD, 1);  // LOAD 0 to latch
  digitalWrite(LOAD, 0);  // set LOAD 1 to finish
}

unsigned char pixbuf[8];

void MAX7219Pset(unsigned char y, unsigned char x, unsigned int state) {
  unsigned char row;

  row = y & 15;
  state &= 1;

  if(state) {
    pixbuf[row] |= 1<<x;
  } else {
    pixbuf[row] &= ~(1<<x);
  }

  MAX7219Send(row+1, pixbuf[row]);
}

void MAX7219Row(unsigned char y, unsigned char x) {
  pixbuf[y] = x;
  MAX7219Send(y+1, pixbuf[y]);
}

void MAX7219Clear() {
  unsigned char i;

  for(i=0;i<8;i++) {
    pixbuf[i] = 0;
    MAX7219Send(i+1, 0);
  }
}


void MAX7219Test() {
  unsigned char x;
  unsigned char y;

  for(y=0;y<8;y++) {
    for(x=0;x<8;x++) {
      MAX7219Pset(y, x, 1);
      delay(DELAY_TIME);
    }
  }
  for(y=0;y<8;y++) {
    for(x=0;x<8;x++) {
      MAX7219Row(y, 0xFF >> x);
      delay(DELAY_TIME);
    }
    MAX7219Row(y, 0);
    delay(DELAY_TIME);
  }
}

void smile() {
  MAX7219Row(0, 0b00000000);
  MAX7219Row(1, 0b00100100);
  MAX7219Row(2, 0b00000000);
  MAX7219Row(3, 0b00000000);
  MAX7219Row(4, 0b10000001);
  MAX7219Row(5, 0b01000010);
  MAX7219Row(6, 0b00111100);
  MAX7219Row(7, 0b00000000);
}

void boom() {
  unsigned char y;
  unsigned char x;
  
  for(y=0;y<8;y++) {
    for(x=0;x<8;x++) {
      MAX7219Row(x, firework[(y*8)+x]);
    }
    delay(DELAY_TIME);
  }
}

int main (void) {
  unsigned char i;

  printf ("Max7219 Test using WiringPi\n");

  if (wiringPiSetup () == -1) exit (1) ;

  pinMode(DATA, OUTPUT);  
  pinMode(CLOCK, OUTPUT);
  pinMode(LOAD, OUTPUT);  

  for(i=0;i<8;i++) {
    pixbuf[i] = 0;
  }
   
  MAX7219Send(SCAN_LIMIT, 7); 
  MAX7219Send(DECODE_MODE, 0); 
  MAX7219Send(DISPLAY_TEST, 0); 
  MAX7219Send(INTENSITY, 1); 
  MAX7219Send(SHUTDOWN, 1);      // turn on the digits
 
  MAX7219Clear();
  getchar();
  
  while(1) {
    for(i=0;i<8;i++) {
      boom();
      delay(rand() % 200);
    }
//    delay(1000);
    smile();
    delay(1000);
//    MAX7219Clear();
//    MAX7219Test();
  }


  return 0;
}
