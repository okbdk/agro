#include "DHT.h" // библа датчика темп+влаж

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float h; // глобальная переменная влажности/ флоат - с плавающей точкой
float t; //глобалка температуры/ они глобальные? так как вызываем в разных функциях

unsigned long SensorsInterval = 3*1000; // интервал для чексенсора, 3 сек (снимаю показания)
unsigned long ActionsInterval = 10*1000; // интервал для функции действия от инфы сенсоров, 10 секунд (по идее для релюх)
unsigned long SensorsFuture; // следующий момент времени когда проверять датчики
unsigned long ActionsFuture; // следующий момент времени когда что-то делать

int pinBuzz = 7; //пин для базера (здесь будут релюхи)
int pinSoil = A0; // входной пин для влажности почвы
int Soil; // значение датчика влажности

void SensorsInit(void) { // однократная инициализация всех сенсоров (добавить)
  pinMode(pinBuzz, OUTPUT);
  pinMode(pinSoil, INPUT);
  dht.begin();
  SensorsFuture = millis() + SensorsInterval; // вычисление момента времени для следующего старта
}

void ActionsInit(void) { //однократная настройка всех пинов под действия (реле в тч, реле нужно сразу показать изначальное известное состояние)
 
  ActionsFuture = millis() + ActionsInterval; // вычисление момента времени когда надо чето поработать
}

void CheckSensors(void) { // запускаем функцию проверки по расписанию интервала
  if (SensorsFuture <= millis()) { // вот он, интервал
    SensorsFuture = millis() + SensorsInterval;
    // измерения
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (isnan(t) || isnan(h)) { // обработка ошибки чтения датчика
      Serial.println("Failed to read from DHT"); // печать ошибки в монитор
      tone(pinBuzz, 440, 500); // жужжалка
        
    
    } else { // если вс> ок, выводить данные с датчиков
      Serial.print("Humidity: "); 
      Serial.print(h);
      Serial.print(" \n");
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.println(" *C ");
    }
  
      Soil = analogRead(pinSoil); // второй датчик влажности почвы
      Serial.print("Soil: "); // выводим данные в монитор
      Serial.println(Soil);
      
      // передача в ESP

      // запись на SD
    
  }    
  
}

void DoActions(void) { // управляем реле
  if (ActionsFuture <= millis()) { // по интервалу
    ActionsFuture = millis() + ActionsInterval; // рассчитываем следующее время срабатывания
    // собственно, действия
    if (h >= 80.0) {
      tone(pinBuzz, 1000, 500); // пошуршали
    }
    if (Soil >= 100) {
      tone(pinBuzz, 440, 500);
    }
  }
}

void setup() { // отрабатывает 1 раз
  Serial.begin(115200); //отладка
  // Serial3.begin(115200);

  SensorsInit(); // настраиваем все датчики
  ActionsInit(); // настраиваем все реле
}

void loop() { //программа
  CheckSensors();
  DoActions();

}
