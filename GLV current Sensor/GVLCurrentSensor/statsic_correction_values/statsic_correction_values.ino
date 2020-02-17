#include <mcp_can.h>
#include <SPI.h>

double calC1 = 0;
double calC2 = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(A2, INPUT);
  //Channel 2 = a = A3
  pinMode(A3, INPUT);
  Serial.begin(9600);
  // put your setup code here, to run once:

//while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
//    {
//        Serial.println("CAN BUS Shield init fail");
//        Serial.println(" Init CAN BUS Shield again");
//        delay(100);
//    }
//    Serial.println("CAN BUS Shield init ok!");
}

void loop() {
  int starttime = millis();
int endtime = starttime;

double intotalC1 = 0;
double intotalC2 = 0;


//subtract these off start values

double numreadings = 0;


//run thsi loop for however long to get evverages to subtract
while ((endtime - starttime) <=2000) // do this loop for up to 1000mS 
{
  int psensorValueC1 = analogRead(A2);
  int psensorValueC2 = analogRead(A3);
  
  double pVoutC1 = (5.0/1023.0) * psensorValueC1;
  double pVoutC2 = (5.0/1023.0) * psensorValueC2;
  
  //constants for the current calculation, provided in datasheet
  int pVc = 5; //same in both calculations
  
  double pGC1 = 0.0667;
  double pGC2 = .0057;
  
  //formula to calculate the current, given in data sheet
  double pcurrentC1 = (pVoutC1 - (pVc/2.0)) * (1.0/pGC1) * (5.0/pVc);
  double pcurrentC2 = (pVoutC2 - (pVc/2.0)) * (1.0/pGC2) * (5.0/pVc);
  
  intotalC1 = intotalC1 + pcurrentC1;
  intotalC2 = intotalC2 + pcurrentC2;

numreadings = numreadings + 1;

endtime = millis();
}
  calC1 = intotalC1 / numreadings;
  calC2 = intotalC2 / numreadings;

Serial.print("c1 offset: ");
  Serial.println(calC1);

 Serial.print("c2 offset: ");
  Serial.println(calC2); 
}
