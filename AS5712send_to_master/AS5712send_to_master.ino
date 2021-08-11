#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN0 = 9;    //設定can0 cs
MCP_CAN CAN0(SPI_CS_PIN0);

unsigned char len = 0;
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

//byte0 電壓 byle1 電流
void read_voltage() {
  float vol ;
  float AcsValue = 0.0, AcsValueF = 0.0;
  String sendcomputer;

  vol = analogRead(A2) * (5.0 / 1023.0) * 10;

  //讀A0的電流值，不可與另一個重複，要用A1之類的
  AcsValue = analogRead(A0);
  AcsValueF = ((( AcsValue / 1024) * 5) - 2.48) * 1000 ; //透過公式換算電流

  int a, b;
  a = vol;
  b = AcsValueF;

  stmp[0] = a;
  stmp[1] = b;
  Serial.print("電壓 : ");
  Serial.println(stmp[0]);
  Serial.print("電流 : ");
  Serial.println(stmp[1]);

  Serial.println("~~~~~~~");

}

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);
  pinMode(A1, OUTPUT);



  CAN0.begin(CAN_500KBPS, MCP_8MHz);
  while (CAN_OK != CAN0.begin(CAN_500KBPS, MCP_8MHz))             // init can bus : baudrate = 500k
  {
    Serial.println("CAN0 BUS Shield init fail");
    Serial.println(" Init CAN0 BUS Shield again");
    delay(100);
  }

  Serial.println("CAN0 BUS Shield init ok!");

  CAN0.init_Mask(0, CAN_EXTID , 0x1fffffff);

}
// 0x1234567 = 19088743

void loop() {

  digitalWrite(A1, HIGH);
  read_voltage();
  delay(500);
  CAN0.sendMsgBuf(0x00000100, 1, sizeof(stmp), stmp);
  delay(5000);


  digitalWrite(A1, LOW);
  read_voltage();
  delay(500);
  CAN0.sendMsgBuf(0x00000100, 1, sizeof(stmp), stmp);

  delay(5000);


}
