#include <iostream>
#include <String>
#include <fstream> 


#ifndef ARDSERIAL
#include "ArduSerial.h"
#define ARDSERIAL
#endif

#define knob_increment 5
WindowsSerial Serial(11);
int applicationNumber = 0;

struct application
{
    std::string name;
    std::string bgName;
    int level = 0.5;
    int red;
    int green;
    int blue;
    boolean sys = false;
} all, spotify, discord;

//application spotify;
application myApps[3];


std::string getLedSeq(int r, int g, int b, int l)
{
    char buffer[10];
    //l = l*12.0/100;
    
    sprintf(buffer,"%03d %03d %03d %03d",r,g,b,l);
    std::string output(buffer);

    return output;
}

std::string getLedSeq(application *myApp) /*int r, int g, int b, int l*/
{
    char buffer[10];
    //int l = myApp->level*12.0/100;
    sprintf(buffer,"%03d %03d %03d %03d",myApp->red,myApp->green,myApp->blue,myApp->level);
    std::string output(buffer);
    std::cout << output << std::endl;

    return output;
}


void setVolume(application *myApp)
{
    
    std::string command;
    if (myApp->sys)
    {
        double nLvl = myApp->level/100.0*65535.0;
        command = "nircmd setsysvolume " + std::to_string(nLvl);
    }
    else
    {
        double nLvl = myApp->level/100.0;
        command = "nircmd setappvolume " + myApp->bgName +" " + std::to_string(nLvl);
    }
    
    system(command.c_str());
    //std::cout << command << std::endl;
}





void setup()
{
    //read files;


    //=========================== Determine Com Ports --------------------
    //std::cout << "Port: " << findComPort();
    


	// Serial11 means access Arduino on COM27 ***************
	Serial.begin(115200);

	std::cout << "Starting..." << std::endl;

	while (!Serial);

	while (Serial.available())
		Serial.read();

    Sleep(4000);
	std::cout << "Connected" << std::endl;

    Serial.print(getLedSeq(0,255,0,12));
    Sleep(250);
    for (int i = 100; i >=0; i-=20 )
    {
        Serial.print(getLedSeq(0,255,0,i));
        Sleep(100);
    }
    Serial.print(getLedSeq(50,50,50,100));
    Sleep(100);
    std::cout << "Standby" << std::endl;
}

void loop()
{
    while (Serial.available())
    {
        char byte = Serial.read();
        if (byte == '1') //vol up
        {
            application *myApp = &myApps[applicationNumber];
            if (myApp->level < 100)
            {
            myApp->level += knob_increment;
            setVolume(myApp);
            Serial.print(getLedSeq(myApp));
            }

        }
        if (byte == '0') //vol down
        {
            application *myApp = &myApps[applicationNumber];
            if (myApp->level > 0)
            {
            myApp->level -= knob_increment;
            setVolume(myApp);
            Serial.print(getLedSeq(myApp));
            }

        }

        if (byte == '2') //change app
        {
            applicationNumber = 1-applicationNumber;
            application *myApp = &myApps[applicationNumber];
            Serial.print(getLedSeq(myApp));
            

        }

        Sleep(100);
    }
}



int main()
{
    
    spotify.name = "spotify";
    spotify.bgName = "spotify.exe";
    spotify.level = 0;
    spotify.red = 0; spotify.green = 255; spotify.blue = 0;

    discord.name = "discord";
    discord.bgName = "discord.exe";
    discord.level = 0;
    discord.red = 255; discord.green = 0; discord.blue = 255;

    all.name = "system";
    all.level = 0;
    all.red = 255; all.green = 255; all.blue = 255;
    all.sys = true;

    myApps[0] = all;
    myApps[1] = spotify;
    myApps[2] = discord;

	setup();
    

	while (true)
		loop();

	return 0;
}