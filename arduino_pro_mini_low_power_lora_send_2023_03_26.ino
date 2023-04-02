/*
VID: 1A86
PID: 7523
 */

#define codeVersion "arduino_pro_mini_low_power_lora_send_2023_03_26"
#define compilationDate __DATE__
#define compilationTime __TIME__
#define analogInput A0 // 12V, 1.27V, av=12109
#define deepSleepTime 60 //MCU deep sleep time,sec

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <LoRa.h>


int av, at;
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

void setup () 
{
  Serial.begin(9600);  
  while (!Serial);
  Serial.println("Setup start...");
  Serial.println(codeVersion);
  Serial.print("Compilation date: ");
  Serial.println(compilationDate);
  Serial.print("Compilation time: ");
  Serial.println(compilationTime); 
  if (!LoRa.begin(433E6)) 
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("... Setup ready");
}

void loop () 
{
  Serial.println("loop start");
/*  for(int i=0;i<1;i++)
  { 
    Serial.print("Start analog reading after ");
    Serial.print(10-i);
    Serial.println(" s");
    delay(999); 
  }
*/
  av=0;
  at=0;
  for(int i=0;i<20;i++) 
  {
    at=analogRead(A0);
    av=av+at;
    Serial.print(i); 
    Serial.print(" "); 
    Serial.print(at); 
    Serial.print(" "); 
    Serial.println(av);
    delay(999);
  }  

  // send packet
  LoRa.idle(); 
  LoRa.beginPacket();
  //LoRa.print("hello Rodopski izvor");
  LoRa.print(av);
  LoRa.endPacket();
  Serial.println("LoRa send end");
  LoRa.end();
  LoRa.sleep();

  
/*  for(int i=0;i<10;i++)
  { 
    Serial.print("MCU deep sleep after ");
    Serial.print(10-i);
    Serial.println(" s");
    delay(999); 
  }
*/
  
 Serial.println("mcu deep sleep start");
 for(int i=0;i<deepSleepTime/8;i++)
 {
    // disable ADC
    //ADCSRA = 0;  
  
    // clear various "reset" flags
    MCUSR = 0;     
    // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit (WDE);
    // set interrupt mode and an interval 
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 1 second delay
    wdt_reset();  // pat the dog
    
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    noInterrupts ();           // timed sequence follows
    sleep_enable();
   
    // turn off brown-out enable in software
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS); 
    interrupts ();             // guarantees next instruction executed
    sleep_cpu ();  
    
    // cancel sleep as a precaution
    sleep_disable();
 }
 Serial.println("mcu deep sleep end");
} // end of loop
