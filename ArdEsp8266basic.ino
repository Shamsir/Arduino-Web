
#define DEBUG true

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);

  sendData("AT+RST\r\n", 2000, DEBUG); // перезагрузка модуля
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); //Переключение режима wifi как точкам доступа
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // полкчение ip от станции
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // установка множественного подключения
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // создание сервера 80
}


float sensetemp() //преобрзование температуры к приемлемому виду
{  
  uint8_t val = (uint8_t)analogRead(A0) << 2;
  return (val/255.0*5);
}

int connectionId;
void loop()
{
  if (Serial1.available())      //получает количество байт(символов) доступных для чтения
  {
    if (Serial1.find("+IPD,"))  //
    {
      delay(300);
      connectionId = Serial1.read() - 48;  //считывает следующий байт
      if (Serial1.find("pin="))
      {
        Serial.println("recieving data from web browser");
        int pinNumber = (Serial1.read() - 48) * 10;
        pinNumber += (Serial1.read() - 48);
        digitalWrite(pinNumber, !digitalRead(pinNumber));
      }
      else
      {
        String webpage = "<h1>Hello World</h1>";
        espsend(webpage);
      }

      if (sensetemp() != 0)
      {
        String add1 = "<h4>Temperature=</h4>";
        String two =  String(sensetemp(), 3);
        add1 += two;
        add1 += "&#x2103"; //////////Hex code for degree celcius
        espsend(add1);
      }

      else
      {
        String c = "sensor is not conneted";
        espsend(c);
      }

      String closeCommand = "AT+CIPCLOSE=";          //закрываем соединение
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}

void espsend(String d)
{
  String cipSend = " AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ","; 
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 1000, DEBUG);
  sendData(d, 1000, DEBUG);
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial1.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
