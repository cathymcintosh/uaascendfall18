#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

#define UNIVERSAL_SCK 9
#define UNIVERSAL_MISO 12
#define UNIVERSAL_MOSI 10
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5

#define BMPaddr1 0x76
#define BMPaddr2 0x77

#define Turbine1Pin A1
#define Turbine2Pin A2

//set Delay Between Data Points
int CollectDelay = 1000;


//I2C Sensor
Adafruit_LSM9DS1 lsmA = Adafruit_LSM9DS1();

//Software-defined SPI
Adafruit_LSM9DS1 lsmB = Adafruit_LSM9DS1(UNIVERSAL_SCK, UNIVERSAL_MISO, UNIVERSAL_MOSI, LSM9DS1_XGCS, LSM9DS1_MCS);

Adafruit_BMP280 bmeA; // first sensor

uint32_t timer;
char Filename[] = "18F000.csv";
int first0Index = 3;
int second0Index = 4;
int third0Index = 5;

void setup() {
  Serial.begin(115200);
  timer = millis();
  
  //Init and find first LSM
  if (!lsmA.begin()) {
        Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
        while (1);
  } else {
        lsmA.setupAccel(lsmA.LSM9DS1_ACCELRANGE_16G);
        lsmA.setupMag(lsmA.LSM9DS1_MAGGAIN_8GAUSS);
        lsmA.setupGyro(lsmA.LSM9DS1_GYROSCALE_2000DPS);
  }
  
  if (!lsmB.begin()) {
        Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
        while (1);
  } else {
        lsmB.setupAccel(lsmB.LSM9DS1_ACCELRANGE_16G);
        lsmB.setupMag(lsmB.LSM9DS1_MAGGAIN_8GAUSS);
        lsmB.setupGyro(lsmB.LSM9DS1_GYROSCALE_2000DPS);
  }

  if (!bmeA.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  //Init the SD card and format the columns
  if (!SD.begin(4))
    {
        Serial.println("SD card initialization failed!");
        //digitalWrite(redLED, HIGH);
        return;
    }
    Serial.println("SD card initialization done.");

    for (int i = 0; i < 1000; i++)
    {
        Filename[first0Index] = (i / 100) + '0';
        Filename[second0Index] = (i % 100) / 10 + '0';
        Filename[third0Index] = i % 10 + '0';
        if (!SD.exists(Filename))
        {
            File dataFile = SD.open(Filename, FILE_WRITE);

            //NOTE: SD formatting in the .csv file requires the headers have no whitespace
            dataFile.println(
              "millis, Date, Hours, Minutes, Seconds, milliseconds,"
              "AccelX1(m/s^2), AccelY1(m/s^2), AccelZ1(m/s^2),"
              "MagX1(gauss), MagY1(gauss), MagZ1(gauss),"
              "GyroX1(dps), GyroY1(dps), GyroZ1(dps),"
              "AccelX2(m/s^2), AccelY2(m/s^2), AccelZ2(m/s^2),"
              "MagX2(gauss), MagY2(gauss), MagZ2(gauss),"
              "GyroX2(dps), GyroY2(dps), GyroZ2(dps),"
              "Temp(C), Pressure(Pa), Altitude(m),"
              "Turbine1Voltage(V)"
              "Turbine2Voltage(V)");
            dataFile.close();
            break; // leave the loop!
        }
    }

    Serial.print("Writing to ");
    Serial.println(Filename);


}

void loop() {
  if (timer > millis()) timer = millis(); //reset if it wraps

  // record data String after CollectDelay
  if (millis() - timer > CollectDelay)
  {
    File dataFile = SD.open(Filename, FILE_WRITE);
    
    //check if dataFile Opend
    if (!dataFile)
    {
        Serial.print("Could not open the SD file: ");
        Serial.println(Filename);
    }


    // put your main code here, to run repeatedly:
      lsmA.read();  /* ask it to read in the data */ 
  
    /* Get a new sensor event */ 
      sensors_event_t a, m, g, temp;
  
      lsmA.getEvent(&a, &m, &g, &temp); 
      RecordData(dataFile, " Accel 1 X: ", a.acceleration.x);
      RecordData(dataFile, " Accel 1 Y: ", a.acceleration.y);
      RecordData(dataFile, " Accel 1 Z: ", a.acceleration.z);
      Serial.println();
      
      RecordData(dataFile, " Mag 1 X: ", m.magnetic.x);
      RecordData(dataFile, " Mag 1 Y: ", m.magnetic.y);
      RecordData(dataFile, " Mag 1 Z: ", m.magnetic.z);
      Serial.println();
      
      RecordData(dataFile, " Gyro 1 X: ", g.gyro.x);
      RecordData(dataFile, " Gyro 1 Y: ", g.gyro.y);
      RecordData(dataFile, " Gyro 1 Z: ", g.gyro.z);
    
      Serial.println();
  
    // SECOND LSM CODE
    lsmB.read();  /* ask it to read in the data */ 
  
    /* Get a new sensor event */ 
  
    lsmB.getEvent(&a, &m, &g, &temp); 
  
    RecordData(dataFile, " Accel 2 X: ", a.acceleration.x);
    RecordData(dataFile, " Accel 2 Y: ", a.acceleration.y);
    RecordData(dataFile, " Accel 2 Z: ", a.acceleration.z);
    Serial.println();

    RecordData(dataFile, " Mag 2 X: ", m.magnetic.x);
    RecordData(dataFile, " Mag 2 Y: ", m.magnetic.y);
    RecordData(dataFile, " Mag 2 Z: ", m.magnetic.z);
    Serial.println();

    RecordData(dataFile, " Gyro 2 X: ", g.gyro.x);
    RecordData(dataFile, " Gyro 2 Y: ", g.gyro.y);
    RecordData(dataFile, " Gyro 2 Z: ", g.gyro.z);
  
    Serial.println();
    


    RecordData(dataFile, " Temperature (C): ", bmeA.readTemperature());
    RecordData(dataFile, " Pressure (Pa): ", bmeA.readPressure());
    RecordData(dataFile, " Altitude (m): ", bmeA.readAltitude(1017)); // 1017 = local PHX hPa
    Serial.println();

    long turbineValue = analogRead(Turbine1Pin);
    RecordData(dataFile, " Turbine 1 Volt (V): ", (turbineValue*3.3)/4095);

    Serial.println();

    turbineValue = analogRead(Turbine2Pin);
    RecordData(dataFile, " Turbine 2 Volt (V): ", (turbineValue*3.3)/4095);
    timer = millis(); // reset the timer
    Serial.println();
    dataFile.println();
    dataFile.close();
  }
}


void RecordData(File dataFile, char* Dataname, float data) {

  Serial.print(Dataname);
  Serial.print(data, 4);
  Serial.print(",");
  dataFile.print(data, 4);
  dataFile.print(",");

}
