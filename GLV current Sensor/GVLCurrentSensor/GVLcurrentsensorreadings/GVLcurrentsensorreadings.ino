

/*Program Description:
 * Using the LEM DHAB S/18 automotive current transducer, current values from the 2 channels
 * are averaged(reduce noise), calculated and returned every .01 seconds
 */


#include <mcp_can.h>
#include <SPI.h>

//static calibration vales
double calC1 = .1;
double calC2 = 2;

/*SAMD core*/


const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);       

void setup() {
  //channel 1 = d = A2
  pinMode(A2, INPUT);
  //Channel 2 = a = A4
  pinMode(A4, INPUT);
  Serial.begin(9600);

  //for callibration at start
  int starttime = millis();
  int endtime = starttime;
  
  //subtract these off start values
  double intotalC1 = 0;
  double intotalC2 = 0;

  //counts readings taken for average
  double numreadings = 0;


//run this loop for however long to get averages to subtract
//will only apply if sensor is not currently reading anything
while ((endtime - starttime) <=2000) // do this loop for up to 1000mS 
{
  int psensorValueC1 = analogRead(A2);
  int psensorValueC2 = analogRead(A4);
  
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

//check to make sure current wasn't running through the sensor while calibration was running, otherwise apply satic correction values
//double check = intotalC1 / numreadings;
//.4 A is the check, c1 filter seems to peak at .3 when no current applied 
  if ( (intotalC1 / numreadings) < .4){
    calC1 = intotalC1 / numreadings;
    calC2 = intotalC2 / numreadings;
  }


  Serial.print("c1 offset: ");
  Serial.println(calC1);

  Serial.print("c2 offset: ");
  Serial.println(calC2); 

  //canstuff
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

}//end of setup



void loop() {
  //READ CURRENT VALUES FROM CHANNELS 1 (+- 30 A)
  //READ CURRENT VALUES FROM CHANNEL 2 (+- 350 A)
  double totalC1 = 0;
  double totalC2 = 0;

  for ( int i = 0; i < 10; i++){
  
    int sensorValueC1 = analogRead(A2);
    int sensorValueC2 = analogRead(A4);
  
    double VoutC1 = (5.0/1023.0) * sensorValueC1;
    double VoutC2 = (5.0/1023.0) * sensorValueC2;
  
    //constants for the current calculation, provided in datasheet
    int Vc = 5; //same in both calculations
  
    double GC1 = 0.0667;
    double GC2 = .0057;
  
    //formula to calculate the current, given in data sheet
    double currentC1 = ((VoutC1 - (Vc/2.0)) * (1.0/GC1) * (5.0/Vc)) ;
    double currentC2 = ((VoutC2 - (Vc/2.0)) * (1.0/GC2) * (5.0/Vc));
  
    totalC1 = totalC1 + currentC1;
    totalC2 = totalC2 + currentC2;
  
    delay(1);
 }//end current value collection

  //taking the average
  double averageC1 = totalC1 / 10.0;
  double averageC2 = totalC2 / 10.0;

  //subtracting offset, giving current current value
  averageC1 = averageC1 - calC1;
  averageC2 = averageC2 - calC2;


  Serial.print("filtered C1: ");
  Serial.print(averageC1);
  Serial.print(",    filtered C2: ");
  Serial.println(averageC2);

  double stmp [2] = {averageC1, averageC2};

  CAN.sendMsgBuf(0x00, 0, 0, 0, 8, stmp);
}
