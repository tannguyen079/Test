
/*------------CONTROL LG AIR CONDITIONER----------*/

#include <boarddefs.h>
#include <IRremote.h>
#include <SPI.h>
#include "RF24.h"

IRsend irsend;
#define IR_LEN 60

const uint64_t pipe = 0xE8E8F0F0E1LL; // адрес канала передачи
RF24 radio(9,10);
byte msg[4];
volatile int count = 0;
int pCount = 0;


const PROGMEM unsigned int Up_Temp[60]={8800, 4100, 550, 1500, 550, 500, 550, 450, 550, 500, 550, 1550, 500, 500, 550, 500, 550, 450, 550, 500, 550, 500, 500, 500, 550, 450, 600, 1500, 500, 550, 500, 500, 550, 500, 550, 1450, 600, 1500, 550, 1550, 500, 1550, 500, 500, 550, 1500, 550, 500, 550, 500, 500, 1550, 550, 450, 550, 1550, 550, 1500, 550}; 

//Decoded LG: 880834F (28 bits)
const PROGMEM unsigned int Down_Temp[60]={8850, 4050, 600, 1450, 550, 500, 550, 450, 550, 500, 550, 1500, 550, 500, 500, 500, 550, 500, 500, 500, 550, 550, 500, 500, 550, 500, 550, 1500, 550, 500, 550, 450, 550, 500, 500, 500, 550, 500, 500, 1550, 500, 1550, 550, 500, 500, 1550, 550, 500, 500, 500, 550, 1500, 550, 1500, 550, 1550, 500, 1500, 550};

//Decoded, LG:, 88C0051
const PROGMEM unsigned int Turn_Off[60]={8900, 4000, 600, 1500, 550, 500, 550, 450, 550, 550, 500, 1500, 550, 500, 550, 500, 500, 500, 500, 1550, 550, 1500, 550, 500, 500, 500, 550, 500, 550, 450, 550, 500, 500, 550, 550, 500, 500, 500, 550, 500, 500, 500, 550, 500, 500, 1550, 550, 450, 550, 1550, 550, 500, 500, 500, 550, 500, 500, 1550, 550}; 

//Decoded, LG:, 880084C
const PROGMEM unsigned int Turn_On[60]={8850, 4100, 550, 1500, 550, 500, 500, 550, 500, 500, 500, 1550, 550, 500, 500, 500, 550, 500, 500, 500, 600, 450, 550, 450, 550, 500, 550, 500, 500, 500, 550, 550, 500, 500, 550, 1500, 550, 500, 500, 550, 500, 500, 550, 500, 500, 1500, 550, 500, 550, 500, 550, 1500, 550, 1550, 500, 500, 550, 500, 500};

//Decoded, LG:, 880A341
const PROGMEM unsigned int Fan[60]={8850, 4050, 600, 1500, 500, 550, 500, 500, 500, 500, 550, 1550, 500, 500, 550, 500, 500, 500, 550, 500, 550, 500, 500, 500, 550, 500, 500, 1600, 500, 500, 500, 1550, 550, 500, 500, 500, 550, 500, 500, 1550, 550, 1500, 550, 500, 500, 1550, 550, 450, 550, 500, 550, 500, 500, 500, 550, 500, 550, 1550, 500};

const PROGMEM unsigned int JETCOOL[60]={8850, 4100, 550, 1500, 550, 500, 500, 500, 550, 500, 500, 1550, 500, 500, 550, 500, 500, 500, 550, 500, 550, 450, 550, 500, 550, 1500, 550, 500, 500, 500, 550, 500, 550, 500, 500, 500, 550, 500, 550, 500, 500, 500, 550, 1550, 500, 500, 550, 500, 500, 500, 550, 1500, 550, 450, 550, 550, 500, 1500, 550};


void IR_Turn_On(void){
  unsigned int ir_buf[IR_LEN];
  for (unsigned int i = 0;  i < IR_LEN;  i++) {
    ir_buf[i] = pgm_read_word_near(Turn_On + i);
  }
  irsend.sendRaw(ir_buf, sizeof(ir_buf)/sizeof(int), 38);
}


void IR_Turn_Off(void){
  unsigned int ir_buf[IR_LEN];
  for (unsigned int i = 0;  i < IR_LEN;  i++) {
    ir_buf[i] = pgm_read_word_near(Turn_Off + i);
  }
  irsend.sendRaw(ir_buf, sizeof(ir_buf)/sizeof(int), 38);
}

void IR_Up_Temp(void){
  unsigned int ir_buf[IR_LEN];
  for (unsigned int i = 0;  i < IR_LEN;  i++) {
    ir_buf[i] = pgm_read_word_near(Up_Temp + i);
  }
  irsend.sendRaw(ir_buf, sizeof(ir_buf)/sizeof(int), 38);
}

void IR_Down_Temp(void){
  unsigned int ir_buf[IR_LEN];
  for (unsigned int i = 0;  i < IR_LEN;  i++) {
    ir_buf[i] = pgm_read_word_near(Down_Temp + i);
  }
  irsend.sendRaw(ir_buf, sizeof(ir_buf)/sizeof(int), 38);
}

void setup(){
  Serial.begin(9600);
  //============================================================Config NRF24
  radio.begin();                      
  radio.setAutoAck(1);                
  radio.setDataRate(RF24_250KBPS);    
  radio.setChannel(10);               
  radio.openReadingPipe(1,pipe);      
  radio.startListening();             
  attachInterrupt(0, interruptFunction, FALLING);  //Create interrupt: 0 for pin 2 or 1 for pin 3, the name of the interrupt function or ISR, and condition to trigger interrupt
}

void loop(){

   if(pCount < count) { //If this is true it means count was interated and another interrupt occurred
    if(msg[0]==10){
        IR_Turn_On();
        Serial.print("Turn_On.. ");
        pCount = count;
    }
    else if (msg[1]==11){
        IR_Turn_Off();
        Serial.print("Turn_Off.. ");
        pCount = count;
      }
    else if (msg[2]==12){
        IR_Up_Temp();
        Serial.print("Up_Temp.. ");
        pCount = count;
      }
    else {
        IR_Down_Temp();
        Serial.print("Down_Temp.. ");
        pCount = count;        
    }
   }
  
}

void interruptFunction() {
 if (radio.available()){
   count++; //up the receive counter
   while(radio.available()) { //get data sent from transmit
         radio.read( &msg, sizeof(msg) ); //read one byte of data and store it in gotByte variable
         Serial.println(msg[0]);
  }
 }

}
