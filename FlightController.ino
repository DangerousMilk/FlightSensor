#include <Wire.h>
#include "MS5611.h"
#include <SD.h>

// Accelerometer
const int ADXL345 = 0x53;
float xOut, yOut, zOut;

// Barometer
MS5611 MS5611(0x77);
float altitude;
float zeroAltitude;
float pressure;
float temperature;

// SD Card
const int chipSelect = 4;

// Log
File logFile;
int logNumber = 0;

// Buzzer
const int buzzer = 8;

// Button
const int buttonPin = 2;
bool recording = false;
bool buttonPressed = false;
float holdStartTime = 0;
float holdTimeNeeded = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Accelerometer setup
  setupADXL345();
  calibrateADXL345();

  // Barometer
  setupMS5611();

  setupSDCard();

  // Successful
  tone(buzzer, 3000, 100);
}

void setupSDCard()
{
  if (!SD.begin(chipSelect)) {
    Serial.println("Failed to initialize SD card");
    while (true)
    {
      errorBeep();
    }
  }
}

void createNewLogFile()
{
  // Iterate over logs to find a free filename
  while (true)
  {
    if(SD.exists(String(logNumber) + ".csv"))
    {
      logNumber += 1;
    }
    else
    {
      logFile = SD.open(String(logNumber) + ".csv", FILE_WRITE);
      break;
    }
  }
}

void setupMS5611()
{
  if (!MS5611.begin()) {
    Serial.println("Failed to initialize MS5611 sensor");
    while (true)
    {
      errorBeep();
    }
  }
  MS5611.reset(1);
}

void setupADXL345()
{
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  delay(10);
}

void calibrateADXL345()
{
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20); // Offset register
  Wire.write(7); // Calibrate = -Round((RealValue - 256) / 4); 
  Wire.endTransmission();
  delay(10);
}

void loop() {
  readAccelerometer();
  readMS5611();
  readButtonState();
  tryLog();

  delay(20);
}

void tryLog()
{
  if(!recording) return;
  if(!logFile) return;

  logFile.print(millis());
  logFile.print(",");
  logFile.print(pressure);
  logFile.print(",");
  logFile.print(altitude - zeroAltitude);
  logFile.print(",");
  logFile.print(temperature);
  logFile.print(",");
  logFile.print(xOut);
  logFile.print(",");
  logFile.print(yOut);
  logFile.print(",");
  logFile.println(zOut);
}

void errorBeep()
{
  tone(buzzer, 1000, 50);
  delay(100);
  tone(buzzer, 1000, 50);
  delay(1000);
}

void readButtonState()
{
  if (digitalRead(buttonPin) == LOW)
  {
    resetButton();
    return;
  }

  if(digitalRead(buttonPin) == HIGH && buttonPressed == false)
  {
    // Start hold
    buttonPressed = true;
    holdStartTime = millis();
  }

  // Check if held down for for long enough
  if(buttonPressed && millis() - holdStartTime >= holdTimeNeeded)
  {
    resetButton();

    if(recording)
    {
      stopRecording();
    }
    else
    {
      zeroAltitude = altitude;
      startRecording();
    }
  }
}

void resetButton()
{
  buttonPressed = false;
  holdStartTime = millis();
}

void startRecording()
{
  recording = true;

  tone(buzzer, 3000, 100);

  createNewLogFile();
  if (logFile) 
  {
    logFile.println("Zeit (ms),Druck (hPa),Höhe (m),Temperatur (C),GX,GY,GZ");
  }
}

void stopRecording()
{
  recording = false;

  tone(buzzer, 1000, 500);

  if (logFile) 
  {
    logFile.close();
  }
}

void readMS5611()
{
  MS5611.read();
  altitude = MS5611.getAltitude();
  pressure = MS5611.getPressure();
  temperature = MS5611.getTemperature();
}

void readAccelerometer()
{
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);

  xOut = readAxis();
  yOut = readAxis();
  zOut = readAxis();
}

float readAxis()
{
  float value = (Wire.read() | Wire.read() << 8);
  return value / 32;
}
