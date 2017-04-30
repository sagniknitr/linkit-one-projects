#include "Wire.h"
#include "I2Cdev.h"
#include "MPU9150.h"
#include "helper_3dmath.h"
#include <LGPS.h>

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69

//********************************ALL OBJECT DECLARATION**************************************

MPU9150 accelGyroMag;
gpsSentenceInfoStruct info;

//*******************************************************************************************

#define M_PI 3.14159265359      
#define dt 0.01             // 10 ms sample rate! 
#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536
#define CRASH_SENSOR_1 1
#define CRASH_SENSOR_2 2
#define ALCOHOL A0
#define LED_PIN 13
#define TIME_UNTIL_WARMUP  20



//------------- ALL GLOBAL VARIABLE-----------------------------------------------------

int16_t a[3];
int16_t g[3];
int16_t m[3];

char buff[256];

unsigned long time;
unsigned long measurement_start;
bool blinkState = false;

bool measurement_mode=false;
bool measurement_done=false;
int alcohol_val=0;
float pitch, roll=0.0f;
bool crash_sensor_1,crash_sensor_2=0;





//------------------------------------------------------------------------------------






void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
     Wire.begin();
     Serial.begin(115200);
     LGPS.powerOn();
     Serial.println("LGPS Power on, and waiting ..."); 
     delay(3000);

//    // initialize serial communication
//    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
//    // it's really up to you depending on your project)
//    Serial.begin(115200);
//
//    // initialize device
//    Serial.println("Initializing I2C devices...");
//    accelGyroMag.initialize();
//
//    // verify connection
//    Serial.println("Testing device connections...");
//    Serial.println(accelGyroMag.testConnection() ? "MPU9150 connection successful" : "MPU9150 connection failed");

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
    pinMode(CRASH_SENSOR_1, OUTPUT);  
    pinMode(CRASH_SENSOR_2, OUTPUT);
    pinMode(ALCOHOL,OUTPUT);
}

void loop() {
    // read raw accel/gyro/mag measurements from device

    time = millis()/1000;
    if (!measurement_mode) {
  
    measurement_mode = true;
    measurement_start = millis()/1000;
    measurement_done = false;
  }

    // Warmup
  if(time<=TIME_UNTIL_WARMUP)
  {
    int progress_time = map(time, 0, TIME_UNTIL_WARMUP, 0, 100);
    //printWarming(progress_time);
  }

  else
  {
    // read Alchol Sensor
    alcohol_val = readAlcohol();

    // read IMU sensor
    accelGyroMag.getMotion9(&a[0], &a[1], &a[2], &g[0], &g[1], &g[2], &m[0], &m[1], &m[2]);
    ComplementaryFilter(a,g);

    // read Crash Sensor
    crash_sensor_1=digitalRead(CRASH_SENSOR_1);
    crash_sensor_2=digitalRead(CRASH_SENSOR_2);

   
  Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  delay(20);

    //const float N = 256;
   // float mag = mx*mx/N + my*my/N + mz*mz/N;

  //  Serial.print(mag); Serial.print("\t");
  //  for (int i=0; i<mag; i++)
       // Serial.print("*");
   // Serial.print("\n");

    // blink LED to indicate activity

  }
}

   
 
void ComplementaryFilter(int16_t accData[3], int16_t gyrData[3])
{
    float pitchAcc, rollAcc;               
    
    // Integrate the gyroscope data -> int(angularSpeed) = angle
    pitch += ((float)gyrData[0] / GYROSCOPE_SENSITIVITY) * dt; // Angle around the X-axis
    roll -= ((float)gyrData[1] / GYROSCOPE_SENSITIVITY) * dt;    // Angle around the Y-axis
 
    // Compensate for drift with accelerometer data if !bullshit
    // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
    int forceMagnitudeApprox = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
    if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
    {
  // Turning around the X axis results in a vector on the Y-axis
        pitchAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;
        pitch = pitch * 0.98 + pitchAcc * 0.02;
 
  // Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
        roll = roll * 0.98 + rollAcc * 0.02;
    }

    Serial.print(roll);
    Serial.print(" ");
    Serial.println(pitch);
} 

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}
 int readAlcohol()
 {
   
  // Number measurements
  int nb_measurements = 5;
  int measurements;

  // Measure
  for (int i = 0; i < nb_measurements; i++) {
    measurements = measurements + analogRead(ALCOHOL); 
  } 
  measurements = measurements/nb_measurements;
  
  return measurements;
 }

