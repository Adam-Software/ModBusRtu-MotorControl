#include <EEPROM.h>
#include<ModbusRtu.h>        //Library for using Modbus in Arduino
#include <SoftwareSerial.h> // Подключаем библиотеку SoftwareSerial для общения с модулем по программной шине UART


#define DIR1 3                               //Определяем выход 3 как направление вращения 
#define DIR2 8                              //Определяем выход 8 как направление вращения
#define MOTOR1 5                           //используем 5 вывод для управления DC-мотором с помощью ШИМ
#define MOTOR2 6                          //используем 6 вывод для управления DC-мотором с помощью ШИМ
#define PLS1  4                          //Настраиваем 4 вход обратной связи мотора 5 - иплуьсы от датчика Хола
#define PLS2  7                         //Настраиваем 7 вход обратной связи мотора 6 - иплуьсы от датчика Хола
#define SPEED 57600                   //Определяем скорость устройства

int ID_Slave = 1;

int PlsState1 = 0;                             // Состояние обратной связи 4
int PlsState2 = 0;                            // Состояние обратной связи 7
int count1 = 0;                              // Аккумулятор счетчика импульсов 1
int count2 = 0;                             // Аккумулятор счетчика импульсов 2

int rpm1 = 0;                                // Устанавливаем счётчик оборотов в 0
int rpm2 = 0;                               // Устанавливаем счётчик оборотов в 0

int flag1=0;                                 // Устанавливаем флаг в 0
int flag2=0;                                // Устанавливаем флаг в 0

int State1 = 0;                              // Устанавливаем состояние направления в 0
int State2 = 0;                             // Устанавливаем состояние направления в 0

int ClrCounter = 0;                         // Устанавливаем состояние команды очистки счетчика в 0

int val1 = 0;                                // Устанавливаем переменную масштабирования значения ШИМ в 0
int val2 = 0;                               // Устанавливаем переменную масштабирования значения ШИМ в 0

int pwm1 = 0;                                // Устанавливаем переменную значения ШИМ в 0
int pwm2 = 0;                               // Устанавливаем переменную значения ШИМ в 0

SoftwareSerial softSerial(11,12);     //Задаем RX, TX
Modbus bus;                          //Определить шину объекта для класса Modbus 

uint16_t au16data[11]; // массив данных modbus
int8_t state = 0;

void setup()
{
  pinMode(PLS1, INPUT_PULLUP);                  //конфигурируем 4 вход как вход и включаем внутренний подтягивающий резистор
  pinMode(PLS2, INPUT_PULLUP);                 //конфигурируем 7 вход как вход и включаем внутренний подтягивающий резистор
  pinMode(DIR1,OUTPUT);                       //конфигурируем 3 вход как выход
  pinMode(DIR2,OUTPUT);                      //конфигурируем 8 вход как выход
  pinMode (MOTOR1, OUTPUT);                 //конфигурируем 5 вход как выход 
  pinMode (MOTOR2, OUTPUT);                //конфигурируем 6 вход как выход 
  
 // EEPROM.put(9, 25);                      //при первой загрузке расскоментировать для записи адреса в EEPROM, потом закоментировать и прошить повторно
 
  EEPROM.get(9, ID_Slave);              //Определяем ID устройства
  au16data[9] = ID_Slave;

  bus = Modbus(ID_Slave,0,0);              //Открываем связь в качестве ведомого   
  softSerial.begin(SPEED);                //Запускаем UART
  bus.begin(&softSerial, SPEED);         //Запускаем Modbus          
}

void loop()
{
  state = bus.poll(au16data, 11);
 
  State1 = au16data[0]; 

  pwm1 = au16data[1]; 
  int val1 = map(pwm1, 0, 1023, 0, 255);        //пропорционально преобразуем измеряемый диапазон значений в выдаваемый диапазон значений
  analogWrite(MOTOR1, val1);                   //выдаем на 5 выход преобразованное значение 

  State2 = au16data[2];

  pwm2 = au16data[3]; 
  int val2 = map(pwm2, 0, 1023, 0, 255);        //пропорционально преобразуем измеряемый диапазон значений в выдаваемый диапазон значений
  analogWrite(MOTOR2, val2);                   //выдаем на 6 выход преобразованное значение 

  ClrCounter = au16data[4];                   //Считывем значение команды очистки счетчика ипульсов с регистра Modbus

  au16data[5] = count1;                      //Записываем значение счетчика импульсов 1 в регистр Modbus
  au16data[6] = count2;                     //Записываем значение счетчика импульсов 2 в регистр Modbus

  au16data[7] = rpm1;                      //Записываем значение счетчика оборотов мотора 1 в регистр Modbus
  au16data[8] = rpm2;                     //Записываем значение счетчика оборотов мотора 2 в регистр Modbus
  
  if (au16data[4] == 3) {
  ID_Slave = au16data[9];
  EEPROM.put(9, ID_Slave);
  }

if (State1 == 0)    
  {
    digitalWrite(DIR1,LOW);    //Направление вращения мотора по умолчанию
 
  }
  if (State1 == 1)
  {
     digitalWrite(DIR1,HIGH);  //Изменяем направление вращения мотора
  } 
 if (State2 == 0)   
  {
    digitalWrite(DIR2,LOW);   //
  }
  if (State2 == 1)
  {
     digitalWrite(DIR2,HIGH);  //
  }
  if (ClrCounter == 1) {
      count1 = 0;             //Очистка значения счетчика импульсов 1
  }

  if (ClrCounter == 2) {
      count2 = 0;           //Очистка значения счетчика импульсов 2
  }

// Считаем импульсы от датчиков Хола мотора 5 и 6 
PlsState1 = digitalRead(PLS1);
  if (!PlsState1 == HIGH && flag1==0 && State1==1) {
        flag1=1;
        count1=count1+1;
        rpm1=count1/211.2;
       
      }
      if (!PlsState1 == LOW && flag1==1 && State1==1) {
        flag1=0;

      }
  if (!PlsState1 == HIGH && flag1==0 && State1==0) {
        flag1=1;
        count1=count1-1;
        rpm1=count1/211.2;
       }
      if (!PlsState1 == LOW && flag1==1 && State1==0) {
        flag1=0;
      }

  PlsState2 = digitalRead(PLS2);
  if (!PlsState2 == HIGH && flag2==0 && State2==1) {
        flag2=1;
        count2=count2+1;
        rpm2=count2/211.2;
   
      }
      if (!PlsState2 == LOW && flag2==1 && State2==1) {
        flag2=0;

      }
  if (!PlsState2 == HIGH && flag2==0 && State2==0) {
        flag2=1;
        count2=count2-1;
        rpm2=count2/211.2;
                    
       }
      if (!PlsState2 == LOW && flag2==1 && State2==0) {
        flag2=0;

      } 
}