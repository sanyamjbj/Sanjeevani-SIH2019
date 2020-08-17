#include <SoftwareSerial.h>
#define MQ7 A0
#define MQ135 A1
#define solenoid 4
#define buzz 3
#define r2_mq7 705
//#define r0_mq7 2000
#define r0_mq7 4600
#define r2_mq135 690
#define r0_mq135 2600
#define b 0.42
#define m -0.263
#define NH3_lim 75
#define CO_lim 175
//#define oximeter

SoftwareSerial BTSerial(10,11);

void setup() {
  // put your setup code here, to run once:
  pinMode(MQ7 , INPUT);
  pinMode(MQ135 , INPUT);
  BTSerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int mq7;
  int mq135;
  mq7 = analogRead(MQ7);
  mq135 = analogRead(MQ135);
  Serial.print("mq7: ");
  Serial.println(mq7);
  Serial.print("mq135: ");
  Serial.println(mq135);
  for(int i=1 ; i<50 ; i++){
    mq7 += analogRead(MQ7);
    mq135 += analogRead(MQ135);
  }
  
  mq7 = mq7/50;
  mq135 = mq135/50;
  
  int CO_ppm = pow( (1538.46*r2_mq7*( (1023/mq7) - 1)/r0_mq7) , -1.709 );
  int NH3_ppm = pow( 10 , ( ( (log10(r2_mq135*(1023/mq135 - 1)/r0_mq135) - b ) )/ m) );
  
  Serial.print("CO_ppm = ");
  Serial.println(CO_ppm);
  Serial.print("NH3_ppm = ");
  Serial.println(NH3_ppm);
 
  if(NH3_ppm>=NH3_lim || CO_ppm>=CO_lim){
     BTSerial.write("ALERT");
     digitalWrite(buzz , HIGH);
     int x = CO_ppm;
     int y = NH3_ppm;
     
     while(x>=CO_lim || y>=NH3_lim){
       BTSerial.write("ALERT");
       digitalWrite(solenoid , HIGH);
       delay(100);
       digitalWrite(solenoid , LOW);
       x = pow( (1538.46*r2_mq7*( (1023/analogRead(MQ7)) - 1)/r0_mq7) , -1.709 );
       y = pow( 10 , ( ( (log10(r2_mq135*(1023/analogRead(MQ135) - 1)/r0_mq135) - b))/ m) );
     }
  }
  
  //BT transmission of data
  else{
    char arr1[7] = {'C' , 'O' , ':' , '0' , '0' , '0' , '\0'};
    char arr2[7] = {'N' , 'H' , ':' , '0' , '0' , '0' , '\0'};
    
    arr1[3] = (CO_ppm/100 + 48);
    arr1[4] = ((CO_ppm-((arr1[3]-48)*100))/10)+48;
    arr1[5] = (CO_ppm-((arr1[3]-48)*100)-((arr1[4]-48)*10))+48;

    arr2[3] = (NH3_ppm/100 + 48);
    arr2[4] = ((NH3_ppm-((arr2[3]-48)*100))/10)+48;
    arr2[5] = (NH3_ppm-((arr2[3]-48)*100)-((arr2[4]-48)*10))+48;

    BTSerial.println(arr1);
    delay(20);
    BTSerial.println(arr2);
    delay(20);
    BTSerial.println("SAFE");
  }  
}
