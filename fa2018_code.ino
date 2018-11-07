Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5

void setupLSM9DS1() {
  // put your setup code here, to run once:
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
}

void loop() {
  // put your main code here, to run repeatedly:

}
