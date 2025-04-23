
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

byte potentiMetrPin1 = 0;
int potentiMetrValue1 = 0;
byte potentiMetrPin2 = 6;
int potentiMetrValue2 = 0;
int nullValue = 510;

RF24 radio(7, 8); // "создать" модуль на пинах 9 и 10 Для ардуино
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x70);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
}

void loop(void) {

  potentiMetrValue1 = analogRead(potentiMetrPin1);
  potentiMetrValue2 = analogRead(potentiMetrPin2);
  int data [] = {processData(potentiMetrValue1), processData(potentiMetrValue2)};
  radio.write(&data, sizeof(data));
}

int processData(int receivedData) {
  int data;
  if (((receivedData > nullValue) && (receivedData < nullValue + 50)) || ((receivedData < nullValue) && (receivedData > nullValue - 50))) {
    return 0;
  }
  if (receivedData > nullValue) {
    if (receivedData > 1020) {
      return -255;
    }
    data = receivedData - nullValue;
    return -data/2;

  } else if (receivedData < nullValue) {
    data = nullValue - receivedData;
    return data/2;
  }
  return 0;
}
