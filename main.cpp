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
};

std::vector<application> myApps;


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
    output += " " + myApp->name;
    //std::cout << output << std::endl;

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

    //setPort();
    //=========================== Determine Com Ports --------------------
    //std::cout << "Port: " << findComPort();
    //mySerial(11);


	// Serial11 means access Arduino on COM27 ***************
	mySerial.setPort(11);
    mySerial.begin(115200);

	std::cout << "Starting..." << std::endl;

	while (!mySerial);

	while (mySerial.available())
		mySerial.read();

    Sleep(4000);
	std::cout << "Connected" << std::endl;

    mySerial.print(getLedSeq(0,255,0,12));
    Sleep(250);
    for (int i = 100; i >=0; i-=20 )
    {
        mySerial.print(getLedSeq(0,255,0,i));
        Sleep(100);
    }
    mySerial.print(getLedSeq(50,50,50,100));
    Sleep(100);
    std::cout << "Standby" << std::endl;
}

void loop()
{
    while (mySerial.available())
    {
        char byte = mySerial.read();
       // std::cout << "B:" << byte << std::endl;
        if (byte == '1') //vol up
        {
            application *myApp = &myApps[applicationNumber];
            if (myApp->level < 100)
            {
            myApp->level += knob_increment;
            setVolume(myApp);
            mySerial.print(getLedSeq(myApp));
            }

        }
        if (byte == '0') //vol down
        {
            application *myApp = &myApps[applicationNumber];
            if (myApp->level > 0)
            {
            myApp->level -= knob_increment;
            setVolume(myApp);
            mySerial.print(getLedSeq(myApp));
            }

        }

        if (byte == '2') //change app
        {
            if (applicationNumber < myApps.size()-1) {applicationNumber += 1;}
            else {applicationNumber =0;}
            application *myApp = &myApps[applicationNumber];
            mySerial.print(getLedSeq(myApp));
            

        }

        Sleep(100);
    }
}


//preconditoins: none
//postconditoins: loads config file into application array struct
void setupApps()
{
    std::cout << "setting up apps" << std::endl;
    std::fstream configFile;
    configFile.open("application.conf",std::ios::in);
    if (configFile.is_open())
    {
        std::string tp;
        while (getline(configFile, tp))
        {
            //std::cout << tp << std::endl;
            
            std::vector<std::string> substrings;
            std::stringstream ss(tp);
            std::string substring;
            while (std::getline(ss, substring, ',')) 
            {
                substrings.push_back(substring);
            }

            application newApp;
            newApp.name = substrings[0];
            newApp.red = stoi(substrings[1]);
            newApp.green = stoi(substrings[2]);
            newApp.blue = stoi(substrings[3]);
            newApp.bgName = substrings[4];
            myApps.push_back(newApp);

            
        }

    }
     std::cout << "apps Loaded" << std::endl;
}

int main()
{
    /*
    spotify.name = "spotify";
    spotify.bgName = "spotify.exe";
    spotify.level = 0;
    spotify.red = 0; spotify.green = 255; spotify.blue = 0;

    discord.name = "discord";
    discord.bgName = "discord.exe";
    discord.level = 0;
    discord.red = 255; discord.green = 0; discord.blue = 255;

    

    myApps[0] = all;
    myApps[1] = spotify;
    myApps[2] = discord;
    */

	setup();
    setupApps();
    //add system
    application all;
    all.name = "system";
    all.level = 0;
    all.red = 255; all.green = 255; all.blue = 255;
    all.sys = true;
    myApps.push_back(all);
    

	while (true)
		loop();

	return 0;
}