#include <banc.h>
#include <Servo.h>


/// Variables globales /

rpm_data rpm;          
thrust_data thrust;    
power_data power;      

unsigned int nbr_sample =100;
unsigned int timeout =40;
byte state_s;
mode_enum mode=CONFIGURATION;


char trame[50];
char buffer_in[10];
char buffer_out[4];
int byte_avail=0;

unsigned int esc_value =900;
Servo esc;

unsigned long tempo;
unsigned int sampling_delay=50;

void setup() {
 Serial.begin(115200);
 init_pins();
 init_rpm();
 esc.attach(ESC_OUT);
 delay(100);
 esc.writeMicroseconds(esc_value);
}

void loop() {
  
  if(mode==CONFIGURATION)
  {   
       byte_avail =Serial.available();
       if(byte_avail)
       {
         Serial.readBytes(buffer_in,byte_avail);    
         decode_trame(buffer_in,&mode,&sampling_delay,&nbr_sample,&timeout);
       }  
   
     getState_sensors(&state_s);
     sprintf(buffer_out,"%03d",state_s);
     Serial.print(buffer_out);
     delay(50);
  }
    
   else if(mode==ACQUISITION)
  {
    tempo = millis();
    while((millis()-tempo) < sampling_delay)
    {
    byte_avail =Serial.available();
    Serial.readBytes(buffer_in,byte_avail);    
    esc_value = atoi(buffer_in);
    if(esc_value == 0)
     {
       esc.writeMicroseconds(ESC_LOW);
       mode = CONFIGURATION;
     }
     else esc.writeMicroseconds(esc_value); //ESC
    }
    // lecture
    getState_sensors(&state_s);
    rpm = read_rpm(nbr_sample,timeout);
    thrust = read_thrust();
    power =  read_power();
    
    build_data_string(trame,&rpm,&thrust,&power,&state_s);
    Serial.print(trame);
  }
  
}
