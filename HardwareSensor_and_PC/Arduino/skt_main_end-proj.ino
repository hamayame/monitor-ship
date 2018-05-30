//constant val
const char ID_HW[] = "C1";
const char ID_SHIP[] = "HS150";

//===============
//import library sensor DHT11
#include "DHT.h"
#define DHTTYPE DHT22
#define DHTPIN 7

//define DHT
DHT dht(DHTPIN, DHTTYPE);

//import library json
#include "ArduinoJson.h"

// set pool json data
StaticJsonBuffer<304> jsonBuffer;

// assign json object
JsonObject& dataSense = jsonBuffer.createObject();

// import library sensor MPU9250
#include "MPU9250_asukiaaa.h"

//define MPU6050
MPU9250 mpu;

// orientation/motion vars
float aX, aY, aZ, aSqrt, mDirection, mX, mY, mZ;
float pitch, roll;

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();

  //start nRF comm
  //radio.begin();
  //radio.openWritingPipe(addressWS);
  //radio.setPALevel(RF24_PA_MIN);
  //radio.stopListening();

  //init serial comm
  Serial.begin(9600);
  while (!Serial) continue;

  //init sensor MPU9250
  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginMag();

  //set offset magneto val
  mpu.magXOffset = -37;
  mpu.magYOffset = -19;
  mpu.magZOffset = -244;

  // start DHT sensing
  dht.begin();
}

void loop() {
  // reading temperature
  float temp = dht.readTemperature();

  //const char text[] = "Hai";
  //radio.write(&text, sizeof(text));

  // get value accel
  mpu.accelUpdate();
  aX = mpu.accelX();
  aY = mpu.accelY();
  aZ = mpu.accelZ();

  // get value magneto and horizontal direction
  mpu.magUpdate();
  mDirection = mpu.magHorizDirection();

  // calculate euler angle [pitch and roll] from accelerometer
  pitch = atan2 (aY, (sqrt(pow(aX, 2) + pow(aZ, 2))));
  roll = atan2(-aX, (sqrt(pow(aY, 2) + pow(aZ, 2))));

  pitch = pitch * 57.3;
  roll = roll * 57.3;

  if (mDirection < 0)
  {
    mDirection += 361;
  }

  // jsonify data
  dataSense["id_dev"] = ID_HW;
  dataSense["id_ship"] = ID_SHIP;
  //dataSense["ori_p"] = ypr[1] * 180 / M_PI;
  dataSense["ori_p"] = pitch;
  //dataSense["ori_r"] = ypr[2] * 180 / M_PI;
  dataSense["ori_r"] = roll;
  dataSense["ori_comp"] = (int) mDirection;
  dataSense["temp"] = temp;

  // send data through serial conn
  dataSense.printTo(Serial);
  //Serial.println(text);
  Serial.println();
  delay(1000);
}

