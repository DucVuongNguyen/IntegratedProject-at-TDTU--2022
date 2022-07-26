
// Thư viện: https://github.com/rkoptev/ACS712-arduino
#include <Arduino.h>
#define ACS712_PIN 34
#define ADC_SCALE 4095
#define VREF 3.3
#define sensitivity 0.066
#define frequency 50

float I_rms, ZeroPoint, I_zero;

void calibrate()
{

  float period = 1000000 / frequency;
  float t_start = micros();

  float ADCsum = 0, measurements_count = 0;
  float ADCnow;

  while (micros() - t_start < 100 * period)
  {

    ADCnow = analogRead(ACS712_PIN);
    Serial.println(ADCnow);
    ADCsum += ADCnow * ADCnow;
    measurements_count++;
  }

  float zero = sqrt(ADCsum / measurements_count);
  ZeroPoint = zero;
  Serial.print("ZeroPoint: ");
  Serial.println(ZeroPoint);
}

float getCurrentAC()
{
  float period = 1000000 / frequency;
  float ADCsum = 0, measurements_count = 0;
  float ADCnow;
  while (analogRead(ACS712_PIN) <= ZeroPoint || analogRead(ACS712_PIN) >= ZeroPoint + 1)
  {
  }
  float t_start = micros();

  while (micros() - t_start < 100 * period)
  {
    ADCnow = analogRead(ACS712_PIN) - ZeroPoint;
    Serial.println("ADCnow: ");
    Serial.println(analogRead(ACS712_PIN));
    ADCsum += ADCnow * ADCnow;
    measurements_count++;
  }
  Serial.println("measurements_count");
  Serial.println(measurements_count);

  float Irms = (sqrt(ADCsum / measurements_count) / ADC_SCALE * VREF / sensitivity) * 1000;
  return Irms;
}

void setup()
{

  Serial.begin(9600);
  adcAttachPin(34);
  delay(5000);
  adcAttachPin(ACS712_PIN);
  Serial.println("Đảm bảo không có dòng điện đi qua cảm biến trong quá trình cân bằng");
  Serial.println("Calibrating...");
  calibrate();
  I_zero = getCurrentAC();
  // Serial.print("I_zero: ");
  // Serial.print(I_zero);
  // Serial.println("mA");
  // Serial.println("Quá trình cân bằng hoàn tất!!!");
}

void loop()
{
  // float I_rms = getCurrentAC(); //Đo dòng AC
  // if (I_rms > I_zero)
  // {
  //   I_rms = I_rms - I_zero + 50;
  // }
  // else
  // {
  //   I_rms = 0;
  // }
  // Serial.print("I_zero: ");
  // Serial.print(I_zero, 4);
  // Serial.println("mA");
  // Serial.print("I_rms = ");
  // Serial.print(I_rms, 4);
  // Serial.println(" mA");
  // Serial.println("-----------");
}