//приемник (RX)

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>
#include <TEA5767Radio.h>

TEA5767Radio radioFM = TEA5767Radio();

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги


byte recieved_data[8]; // массив принятых данных
byte relay[8] = {2,3,4,5,6,7,8,14};

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void first_step()
{
	byte chnl = 0x60;
	byte pipeNo;
	while ( radio.available(&pipeNo)) 
	{	
		char chnl[256]; 
		radio.read( &chnl, sizeof(chnl) );
		float fm = atof(chnl);
		if( fm != 0)
		{
			
			char str[256];
			sprintf(str,"recieved %f", fm);
			radio.writeAckPayload(pipeNo,str,256);
			Serial.println("got new channel " + fm);
			break;
		}
	}
	radioFM.setFrequency(fm); // pick your own frequency
	Serial.println("Start working");
}

void setup() 
{
	Serial.begin(9600); //открываем порт для связи с ПК

	Wire.begin();


/*	
	pinMode(relay_02, OUTPUT); // настроить пин реле как выход
	pinMode(relay_03, OUTPUT); // настроить пин реле как выход
	pinMode(relay_04, OUTPUT); // настроить пин реле как выход
	pinMode(relay_05, OUTPUT); // настроить пин реле как выход
	pinMode(relay_06, OUTPUT); // настроить пин реле как выход
	pinMode(relay_07, OUTPUT); // настроить пин реле как выход
	pinMode(relay_08, OUTPUT); // настроить пин реле как выход
*/
	for(int i = 0; i < 8; i++)
		pinMode(relay[i], OUTPUT); // настроить пин реле как выход


	
	radio.begin(); //активировать модуль
	radio.setAutoAck(1);				 //режим подтверждения приёма, 1 вкл 0 выкл
	radio.setRetries(0, 15);		//(время между попыткой достучаться, число попыток)
	radio.enableAckPayload();		//разрешить отсылку данных в ответ на входящий сигнал
	radio.setPayloadSize(32);		 //размер пакета, в байтах

	radio.openReadingPipe(1, address[0]);		 //хотим слушать трубу 0
	radio.setChannel(0x60);	//выбираем канал (в котором нет шумов!)

	radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	//должна быть одинакова на приёмнике и передатчике!
	//при самой низкой скорости имеем самую высокую чувствительность и дальность!!

	radio.powerUp(); //начать работу
	radio.startListening();	//начинаем слушать эфир, мы приёмный модуль
	
}

void loop()
{
	first_step();
	byte pipeNo;
	while ( radio.available(&pipeNo)) 
	{	// слушаем эфир со всех труб
		radio.read( &recieved_data, sizeof(recieved_data) );				 // чиатем входящий сигнал
/*
		digitalWrite(relay_01, recieved_data[0]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_02, recieved_data[1]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_03, recieved_data[2]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_04, recieved_data[3]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_05, recieved_data[4]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_06, recieved_data[5]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_07, recieved_data[6]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
		digitalWrite(relay_08, recieved_data[7]); // подать на реле сигнал с 0 места массива
		//то бишь 0 или 1
*/
		for(int i = 0; i < 8; i++)
			digitalWrite(relay[i],recieved_data[i]);
			
	}
}

