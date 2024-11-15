#include <Wire.h>         // Mở giao tiếp I2C
#include "MAX30100.h"     // Thư viện cho module MAX30100
#include <TinyGPS++.h>    // Thư viện cho module GPS
#include <SoftwareSerial.h>  // Thư viện cho module SIM800L

// Cài chân trên SIM800L
#define SIM800_TX_PIN 4
#define SIM800_RX_PIN 2
SoftwareSerial sim800lSerial(SIM800_TX_PIN, SIM800_RX_PIN);

// Creative GPS
TinyGPSPlus gps;

// Creative MAX30100
MAX30100 heartRateSensor;

void setup() {
  Serial.begin(9600);

  sim800lSerial.begin(9600);

  Wire.begin();

  // Kết nối với module MAX30100
  if (!heartRateSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Không thể tìm thấy MAX30100");
    while (1);
  }
  
  // Đặt chân IO theo đúng mô tả kết nối
  pinMode(22, OUTPUT); // Chân SCL
  pinMode(21, INPUT); // Chân SDA
  pinMode(19, INPUT); // Chân INT
}

// Hàm gửi tin nhắn
void sendSMS(String message) {
  sim800lSerial.println("AT+CMGF=1"); // Chế độ gửi tin nhắn
  delay(100);
  sim800lSerial.println("AT+CMGS=\"+8401234567\""); // Số điện thoại muốn gửi
  delay(100);
  sim800lSerial.println(message);
  delay(100);
  sim800lSerial.println((char)26);
  delay(100);
  sim800lSerial.println();
}


void loop() {
  // Đọc dữ liệu từ GPS
  while (sim800lSerial.available()) {
    gps.encode(sim800lSerial.read());
  }

  // Đo nhịp tim và huyết áp từ MAX30100
  float heartRate = heartRateSensor.getHeartRate();
  float SpO2 = heartRateSensor.getSpO2();

  // Lấy dữ liệu vị trí từ GPS
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();

  // Gửi tin nhắn 
  sendSMS("Nhịp tim: " + String(heartRate) + ", Huyết áp: " + String(SpO2) +
          ", Vị trí: https://www.google.com/maps/search/?api=1&query=" +
          String(latitude) + "," + String(longitude));

  delay(5000); // Chờ 5 giây trước khi gửi tin nhắn tiếp theo
}

