// передатчик (ТХ)

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

/*byte button_01 = 2;	// кнопка на 2 цифровом
byte button_02 = 3;	// кнопка на 3 цифровом
byte button_03 = 4;	// кнопка на 4 цифровом
byte button_04 = 5;	// кнопка на 5 цифровом
byte button_05 = 6;	// кнопка на 6 цифровом
byte button_06 = 7;	// кнопка на 7 цифровом
byte button_07 = 8;	// кнопка на 8 цифровом
byte button_08 = 14;// кнопка на А0 (14) аналоговом*/
byte button[8] = {2,3,4,5,6,7,8,14};

byte transmit_data[8]; // массив, хранящий передаваемые данные
byte latest_data[8]; // массив, хранящий последние переданные данные
boolean flag; // флажок отправки данных
char* chnl; //канал, на котором будет вещание

void send(byte *data, byte len)
{
	radio.powerUp(); // включить передатчик
	radio.write(&data, len); // отправить по радио
	radio.powerDown(); // выключить передатчик
}

void first_step()
{
	int count = 100;
	while(count--)
	{
		send(&chnl,strlen(chnl));
		if(radio.isAckPayloadAvailable())
		{
			char str[256];
			radio.read(&str,sizeof(str));
			Serial.print("someone says: ");
			Serial.println(str);
		}
		if(!digitalRead(button[0]) && !digitalRead(button[2]) && !digitalRead(button[6])) // если нажаты 1, 3 и 7 кнопки - выходим
		{
			Serial.println("You pressed 1,3,7 buttons, start working");
			break;
		}
		delay(500);
	}
	if(count <= 0)
	{
		Serial.println("I will wait no more, start working");
	}
}

void setup() 
{
	Serial.begin(9600); //открываем порт для связи с ПК
	for(int i = 0; i < 8; i++)
		pinMode(button[i], INPUT_PULLUP);

	Serial.print("Hello, enter radio channel (default is 0x60)");
	int counts = 0;
	while(!Serial.available())
	{
		counts++;
		if(!counts%10)
		Serial.println("counts = " + counts);
		if(counts > 100) 
		{
			Serial.println("I will wait no more!!!!");
				break;
			}
		delay(500);
	}
	if(Serial.available())
		chnl = Serial.readString();
	Serial.println("press 1,3,7 buttons to stop sending channel and start work");
	radio.begin(); //активировать модуль
	radio.setAutoAck(1);				 //режим подтверждения приёма, 1 вкл 0 выкл
	radio.setRetries(0, 15);		//(время между попыткой достучаться, число попыток)
	radio.enableAckPayload();		//разрешить отсылку данных в ответ на входящий сигнал
	radio.setPayloadSize(32);		 //размер пакета, в байтах

	radio.openWritingPipe(address[0]);	 //мы - труба 0, открываем канал для передачи данных
	radio.setChannel(0x60);	//выбираем канал (в котором нет шумов!)

	radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	//должна быть одинакова на приёмнике и передатчике!
	//при самой низкой скорости имеем самую высокую чувствительность и дальность!!

	radio.powerUp(); //начать работу
	radio.stopListening();	//не слушаем радиоэфир, мы передатчик
}

void loop() 
{
	first_step();

	for (int i = 0; i < 8; i++) 
	{ // в цикле от 0 до числа каналов
		transmit_data[i] = !digitalRead(button[i]);
		if (transmit_data[i] != latest_data[i]) 
		{ // если есть изменения в transmit_data
			flag = 1; // поднять флаг отправки по радио
			latest_data[i] = transmit_data[i]; // запомнить последнее изменение
		}
	}

	if (flag == 1) 
	{
		send(transmit_data, sizeof(transmit_data));
		flag = 0; //опустить флаг
	}

}
