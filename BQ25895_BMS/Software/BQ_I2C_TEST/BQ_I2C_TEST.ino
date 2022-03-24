
#include<Wire.h>

#define BQ25895 0x6A

char ct[10][46] = {
  {"No Input"},
  {"USB SDP"},
  {"USB CDP(1.5A)"},
  {"USB DCP(3.25A)"},
  {"Adjustable High Voltage DCP (MaxCharge)(1.5A)"},
  {"Unknown Adapter(500mA)"},
  {"Non Standard Adapter"},
  {"OTG"},
};

char Status [4] [12] = { 
 { "No chrg" }, 
 { "Pre chrg" }, 
 { "Fast chrg" }, 
 { "Chrg Done!" },  
};

char PG_Status[2][20] = {
  {"Not Power Good"},
  {"Power Good"},
};

char USB_Input_Status[2][20] = {
  {"USB 100"},
  {"USB 500"},
} ;






void setup(){

    Wire.begin();
    Serial.begin(9600);
    

writeI2CReg(BQ25895, 0x14, B10111001);//full BQ reset
writeI2CReg(BQ25895, 0x02, B11111111);//Satrting ADC conversion
writeI2CReg(BQ25895,0x07,B00001101);// Disable the watchdog timer, disablingg the charge termination 
writeI2CReg(BQ25895,0x0d,B10010010);//force VINDPM and setting the input voltage threshold to 4V4
writeI2CReg(BQ25895,0x00,B00111111);// Setting input current limit to 1.5A and disabling ILIM Pin
writeI2CReg(BQ25895,0x06,B01011110);//Setting charge voltage limit to 4V2
writeI2CReg(BQ25895,0x04,B00000100);//Setting fast charge current to 960mA
writeI2CReg(BQ25895,0x03,B00111010);//Setting minimum system voltage to 3V5
writeI2CReg(BQ25895,0x05,B00110011);//Setting Pre-charge current to 256mA
writeI2CReg(BQ25895,0x09,B01000100);//Enabling BATFET




  
}
void loop(){

    
    Serial.println(ct[chargerTypeDetect()]);
    Serial.println(Status[chargeStatus()]);
    Serial.print("VSYS : ");Serial.println(readVSYS()); 
    Serial.print("VBAT : ");Serial.println(readVBAT());
    Serial.print("VBUS : ");Serial.println(readVBUS());
    Serial.print("Fault Register Reading : "); Serial.println(readI2CReg(BQ25895,0x0c));
    Serial.print("Power good status : "); Serial.println(PG_Status[Power_good_status()]);
    Serial.print("USB Input Satus : "); Serial.println(USB_Input_Status[usb_input_status()]);



    Serial.println("######################################################");
    delay(1000);

}

//######### Method Implementations ##########################

int readI2CReg(uint8_t S_address,uint8_t R_address){

    Wire.beginTransmission(S_address);// Setting the slave device address 
    Wire.write(R_address);//Writing the register address
    Wire.endTransmission();
    
    Wire.requestFrom(S_address,1);//reading 1 byte form slave device 
    uint8_t buf  = (uint8_t)Wire.read();
    return(buf);
        
}

void writeI2CReg(uint8_t S_address,uint8_t R_address,uint8_t data){

    Wire.beginTransmission(S_address);//Setting slave address
    Wire.write(R_address);//Setting register address
    Wire.write(data);//Writing data to the register 
    Wire.endTransmission(); 
}

int chargerTypeDetect(void)
{
  byte readByte;
  readByte  = (byte)readI2CReg(BQ25895, 0x0b); //Read the data from the register
  return (((readByte & B11100000)>>5));

}


int chargeStatus(void){
byte readByte;
readByte = readI2CReg(BQ25895,0x0b);//read data form register 

return((readByte & B00011000)>>3);

}

float readVSYS(void){
  byte readByte ;
  readByte = readI2CReg(BQ25895,0x0f);
  
  return(2.304 + (0.01987*((readByte & B01111111))));
}

float readVBAT(void){
  byte readByte ;
  readByte = readI2CReg(BQ25895,0x0e);
  
  return(2.304 + (0.01987*((readByte & B01111111))));
}

float readVBUS(void){
   byte readByte ;
  readByte  = readI2CReg(BQ25895,0x11);

  return(2.6 + (0.09921875*((readByte & B01111111))));


}

bool Power_good_status(void){

byte readByte;
readByte = readI2CReg(BQ25895,0x0b);//read data form register 

return((readByte & B00000100)>>2);

}

bool usb_input_status(void){

byte readByte;
readByte = readI2CReg(BQ25895,0x0b);//read data form register 

return((readByte & B00000010)>>1);

}





