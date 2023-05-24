import pymodbus
import serial
from pymodbus.pdu import ModbusRequest
from pymodbus.client.sync import ModbusSerialClient as ModbusClient #initialize a serial RTU client instance
from pymodbus.transaction import ModbusRtuFramer
import time

client= ModbusClient(method = "rtu", port="/dev/ttyUSB1",stopbits = 1, bytesize = 8, parity = 'N', baudrate= 9600)
connection = client.connect()
print(connection)

def change_slave_address(address):
    write_registers(4, 3, 22)        #Разрешение перезаписи адреса Slave по адресу 22
    write_registers(9, address, 22)  #Запись нового адреса address = 1-254
                                     #после чего нужно перезагрузить Slave устройство

def reset_counter():
    write_registers(4, 1, 22)  # Очистка счетчика импульсов переднего мотора по адресу 22
    write_registers(4, 2, 22)  # Очистка счетчика импульсов заднего мотора по адресу 22
    write_registers(4, 1, 23)  # Очистка счетчика импульсов переднего мотора по адресу 23
    write_registers(4, 2, 23)  # Очистка счетчика импульсов заднего мотора по адресу 23

    write_registers(4, 0, 22)  # Сброс флага очистки счетчика импульсов переднего мотора по адресу 22
    write_registers(4, 0, 22)  # Сброс флага очистки счетчика импульсов заднего мотора по адресу 22
    write_registers(4, 0, 23)  # Сброс флага очистки счетчика импульсов переднего мотора по адресу 23
    write_registers(4, 0, 23)  # Сброс флага очистки счетчика импульсов заднего мотора по адресу 23

def read_registers(start_address, count, slave_id):
    read_reg = client.read_input_registers(start_address, count, unit=slave_id)
    return read_reg

def write_registers(address, value, slave_id):
    write_reg = client.write_registers(address,value, unit=slave_id)
    return write_reg

def change_dir(dir0, dir1, dir2, dir3):
    write_registers(0, dir0, 22)  # Направление вращения (0-1) переднего мотора по адресу 22
    write_registers(2, dir1, 22)  # Направление вращения (0-1) заднего мотора по адресу 22
    write_registers(0, dir2, 23)  # Направление вращения (0-1) переднего мотора по адресу 23
    write_registers(2, dir3, 23)  # Направление вращения (0-1) переднего мотора по адресу 23

def acc_motor():

  for i in range(102):

    speed = i * 10 #плавно увеличиваем скорость моторов speed = 0-1023

    write_registers(1, speed, 22) #Запуск переднего мотора по адресу 22
    write_registers(3, speed, 22) #Запуск заднего мотора по адресу 22

    write_registers(1, speed, 23) #Запуск переднего мотора по адресу 23
    write_registers(3, speed, 22) #Запуск заднего мотора по адресу 23

    read_reg0 = read_registers(0, 10, 22) #чтение всех регистров от 0 до 10 по адресу 22
    read_reg1 = read_registers(0, 10, 23) #чтение всех регистров от 0 до 10 по адресу 23

    print(read_reg0.registers) #вывод данных массива всех регистров по адрессу 22
    print(read_reg1.registers) #вывод данных массива всех регистров по адрессу 23

    #time.sleep(0.01)

def stop_motor():
    print(write_registers(3, 0, 22))  # Остановка заднего мотора по адресу 22
    print(write_registers(1, 0, 22))  # Остановка переднего мотора по адресу 22
    print(write_registers(3, 0, 23))  # Остановка заднего мотора по адресу 23
    print(write_registers(1, 0, 23))  # Остановка переднего мотора по адресу 23

def main():
  reset_counter()
  change_dir(1, 1, 1, 1)
  acc_motor()
  stop_motor()

main()
