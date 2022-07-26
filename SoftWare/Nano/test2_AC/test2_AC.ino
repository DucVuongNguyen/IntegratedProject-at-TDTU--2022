
//Thư viện: https://github.com/rkoptev/ACS712-arduino

#include "ACS712.h"

ACS712 sensor(ACS712_30A, A0); //Cảm biến 30A, kết nối chân A0
//ACS712_05B
//ACS712_20A
//ACS712_30A
float tong;
float I;
float I_TB;
float ma;
float k;
void setup()
{
  Serial.begin(9600);

  // calibrate cảm biến
  // Trong quá trình cali phải đảm bảo KHÔNG có dòng điện đi qua cảm biến
  Serial.println("Đảm bảo không có dòng điện đi qua cảm biến trong quá trình cân bằng");
  delay(1000);
  Serial.println("Calibrating...");
  delay(200);
  sensor.calibrate();
  for (int i = 0; i <= 20; i++)
  {
    I = sensor.getCurrentAC(); //Đo dòng AC
    //Serial.print("I = "); Serial.print(I); Serial.print("   ");
    tong = tong + I;
    //Serial.print("Tổng = "); Serial.println(tong);
  }
  I_TB = tong / 20;
  k = I_TB;
  Serial.println("Quá trình cân bằng hoàn tất!!!");
}

void loop()
{
  for (int i = 0; i <= 150; i++)
  {
    I = sensor.getCurrentAC(); //Đo dòng AC
    //Serial.print("I = "); Serial.print(I); Serial.print("   ");
    tong = tong + I;
    //Serial.print("Tổng = "); Serial.println(tong);

  }

  //      I_TB = (tong / 100);
  //    tong = 0;
  //    //Serial.print("\t\t\t");
  //    Serial.print("I = "); Serial.print(I_TB,4); Serial.print("A"); Serial.print("   ");
  //    ma = (I_TB) * 1000;
  //    Serial.print("mA = "); Serial.print(ma); Serial.println("mA");
  I_TB = (tong / 150);
  tong = 0;


  if (I_TB > k)
  {
    I_TB = I_TB - 0.022;
  }
  else
  {
    I_TB = 0;
  }
  Serial.print("k = "); Serial.print(k, 4); Serial.println("A");
  Serial.print("I = "); Serial.print(I_TB, 4); Serial.println("A");
  ma = (I_TB) * 1000;
  Serial.print("mA = "); Serial.print(ma); Serial.println("mA");
  Serial.println("-----------");

}
