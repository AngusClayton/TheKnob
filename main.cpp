//use this board: 113991054
#include <iostream>
#include <string>
#include <fstream> 
#include<chrono>
#include <vector>
#include <sstream>

#ifndef ARDSERIAL
#include "ArduSerial.h"
#define ARDSERIAL
#endif

#define knob_increment 5
WindowsSerial mySerial(11); 


bool validateString(const std::string& str) {
    for (char c : str) {
        //std::cout << c << std::endl;
        if (!isalnum(c) && c != '.') {
            
            return false;
        }
    }
    return true;
}

//preconditions none:
//postconditions: scanns all serial ports until it find the knob. does this with a handshake
void setPort()
{
    for (int i = 0; i < 100; i++)
    {
        mySerial.setPort(i);
        mySerial.begin(115200);
        /*
        for (int i=0;i<40;i++)
        {
            if (mySerial.connected()) {break;}
            Sleep(100);
        }*/
        if (mySerial.connected()) 
        {
            std::cout << "found " << i <<std::endl; 
            mySerial.print("KNOB_HANDSHAKE");
            //wait for available
            std::chrono::time_point start = std::chrono::steady_clock::now();
            while (mySerial.available() == 0) 
            {
                mySerial.print("KNOB_HANDSHAKE");
                if(std::chrono::steady_clock::now() - start > std::chrono::seconds(4)) break;
                Sleep(100);
             
            }
            char buffer[14];
            mySerial.read(buffer, 14);
            std::cout << buffer << std::endl;    
            if (buffer[0] == 'K' && buffer[1] == 'N') break; //if buffer == "KNOBAVAILABLE" then break out of loop.
            
            

        }
        mySerial.end();
        
    }
    
}




void setVolume(std::string name, int level)
{
    
    std::string command;
    if (name == "sys")
    {
        double nLvl = level/100.0*65535.0;
        command = "nircmd setsysvolume " + std::to_string(nLvl);
    }
    else
    {
        double nLvl = level/100.0;
        command = "nircmd setappvolume " + name +" " + std::to_string(nLvl);
    }
    
    system(command.c_str());
    //std::cout << command << std::endl;
}

void setup()
{
    

    setPort();

	//mySerial.setPort(11);
    //mySerial.begin(115200);

	std::cout << "Starting..." << std::endl;

	while (!mySerial);

	while (mySerial.available())
		mySerial.read();

    Sleep(4000);
	std::cout << "Connected" << std::endl;


}

void loop()
{
    while (mySerial.available())
    {
        std::string buffer;
        char last = mySerial.read();
        while (last != '\n')
        {
            if (last != ' ') {buffer += last;}
            
            last = mySerial.read();
            
        }
        //std::cout<<buffer;
        std::string name = buffer.substr(0,buffer.find(':')); 
        std::string level = buffer.substr(buffer.find(':')+1,buffer.length() - buffer.find(':')-2);
        std::cout << name << " is now " <<level << std::endl; 

        if (validateString(name) && validateString(level))
        {setVolume(name,stoi(level));}

        else {std::cout << "Invalid input caught!" << std::endl;}
        

    }
    

}



int main()
{


	setup();
    

	while (true)
		loop();

	return 0;
}