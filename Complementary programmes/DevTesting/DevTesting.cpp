// DevTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4507 34)

#include <iostream>
#include "SASLIBdev.hpp"

// ====================== Function headers ====================== 
void testStimulator();

// ====================== main ====================== 
int main()
{
    std::cout << "Hello World!\n";
    testStimulator();

    return 0;
}

// ====================== Function definitions ====================== 
void testStimulator()
{
    RehaMove3 stimulator;
    char PORT_STIM[5] = "COM5"; 
    char PORT_STIM_UI[5] = "COM5";
    int countPort = 0, nrPort = 1;
    bool success = false;
    // initialization

    while (!success)
    {

        // Select port from the GUI
        if (PORT_STIM_UI[3] >= '1' && PORT_STIM_UI[3] <= '9')
        {
            PORT_STIM[3] = PORT_STIM_UI[3];
        }
        countPort++;

        // Connect to the device and initialize settings
        printf("Starting stimulator on port %s, try nr %d, nrPort %d, %s\n", PORT_STIM, countPort, nrPort, PORT_STIM_UI);
        stimulator.display = true;
        stimulator.init(PORT_STIM);
        printf("%s\n", stimulator.displayMsg);

        stimulator.fq[Smpt_Channel_Red] = 30;

        success = stimulator.ready;


        // others
        if (countPort >= 3)
        {
            countPort = 0;

            nrPort++;
            if (nrPort > 9)
            {
                nrPort = 1;
            }
            char cPort[8];
            sprintf(cPort, "%d", nrPort);
            PORT_STIM_UI[3] = cPort[0];
        }

        // loop settings
        Sleep(100);

        
        if (!success)
        {
            stimulator.display = true;
            stimulator.end();
            printf("%s\n-------------------------------\n", stimulator.displayMsg);
        }
        
        

        // loop settings
        Sleep(100);
    }



}