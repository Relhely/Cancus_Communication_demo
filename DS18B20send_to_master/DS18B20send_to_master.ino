#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "mcp_can.h"

OneWire oneWire(4);
DallasTemperature DS18B20(&oneWire);

const int SPI_CS_PIN0 = 9;    //設定can0 cs接腳

MCP_CAN CAN0(SPI_CS_PIN0);

unsigned char len = 0;
unsigned char buf[8];
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
unsigned long atime = millis();
unsigned long btime;

void read_temp() {
  float temperature;
  int temp;
  DS18B20.requestTemperatures();
  temperature = DS18B20.getTempCByIndex(0);
  Serial.print("溫度 : ");
  Serial.println(temperature);
  temp = temperature;
  stmp[0] = temp;
  Serial.println(stmp[0]);
}


void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);  //can0
  pinMode(3, INPUT);  //can1
  pinMode(A5, OUTPUT);


  CAN0.begin(CAN_500KBPS, MCP_8MHz);
  while (CAN_OK != CAN0.begin(CAN_500KBPS, MCP_8MHz))             // init can bus : baudrate = 500k
  {
    Serial.println("CAN0 BUS Shield init fail");
    Serial.println(" Init CAN0 BUS Shield again");
    delay(100);
  }
  Serial.println("CAN0 BUS Shield init ok!");




  CAN0.init_Mask(0, CAN_EXTID , 0x1fffffff);
  CAN0.init_Filt(0, 1, 0x00001100);

}

// 0x1234567 = 19088743

void loop() {

  if (!digitalRead(2)) { //  can1
    CAN0.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
    Serial.println("\r\n------------------------------------------------------------------");
    Serial.print("Get Data From id: ");
    Serial.println(CAN0.getCanId());
    int ag = buf[0];
    Serial.println(ag);
    if (ag == 1) {
      digitalWrite(A5, HIGH);

      Serial.println("發光");
    }
    if (ag == 0){
      digitalWrite(A5, LOW);
    }

  }

  btime = millis();
  if ((btime - atime) >= 5000) {
    read_temp();
    atime = btime;
    CAN0.sendMsgBuf(0x00000101, 1, sizeof(stmp), stmp);
  }


  delayMicroseconds(50);
}
