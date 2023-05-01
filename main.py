import serial
import time
ser = serial.Serial('COM11',115200)
totalLEDS = 4

lvl = input("Lvl: ")
while lvl:
    myStr = "255 000 255 "
    if int(lvl) < 10:
        myStr += "0"
    myStr+=lvl
    print(myStr)
    ser.write(myStr.encode())
        
    
    lvl = input("Lvl: ")
