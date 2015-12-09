//######################################################################################################################
/*
    Copyright (c) since 2014 - Paul Freund

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/
//######################################################################################################################


#include "stdafx.h"
#include <windows.h>
#include "SpheroRAWItf.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

//======================================================================================================================

void PrintDeviceStatus(string action, ISpheroDevice* device) {
    cout << "Action: " << action << " Result: ";

    if(device == nullptr) {
        cout << "Error - Sphero handle is invalid" << endl;
        return;
    }

    switch(device->state()) {
        case SpheroState_None:                          { cout << "SpheroRAW not initialized"                   << endl; break; }
        case SpheroState_Error_BluetoothError:          { cout << "Error - Couldn't initialize Bluetooth stack" << endl; break; }
        case SpheroState_Error_BluetoothUnavailable:    { cout << "Error - No valid Bluetooth adapter found"    << endl; break; }
        case SpheroState_Error_NotPaired:               { cout << "Error - Specified Sphero not Paired"         << endl; break; }
        case SpheroState_Error_ConnectionFailed:        { cout << "Error - Connecting failed"                   << endl; break; }
        case SpheroState_Disconnected:                  { cout << "Sphero disconnected"                         << endl; break; }
        case SpheroState_Connected:                     { cout << "Sphero connected"                            << endl; break; }
    }

    cout << endl;
}

void OrbBasicAppendLine(ISpheroDevice* device, std::string lineText) {
    std::vector<ubyte> program(lineText.begin(), lineText.end());
    program.insert(program.end(), '\n');
    device->appendOrbBasicFragment(0, program);
}

//======================================================================================================================

int _tmain(int argc, _TCHAR* argv[])
{
    //------------------------------------------------------------------------------------------------------------------
    // Create device 
    ISpheroDevice* device = SpheroRAW_Create("Sphero-GYY");

    bool quit = false;

	std::string parameters[42] = { "AccX", "AccY", "AccZ", "giroX", "giroY", "giroZ", "reserved", "reserved", "reserved", "rmbEMF", "lmbEMF", "rmPWM", "lmPWM", "IMUpitch", "IMUroll", "IMUyaw", "AXf", "AYf", "AZf", "giroXfilt", "giroYfilt", "giroZfilt", "reserved", "reserved", "reserved", "rmbEMFfilt", "lmbEMFfilt", "reserved", "reserved", "reserved", "reserved", "reserved", "QO", "Q1", "Q2", "Q3", "odoX", "odoY", "AccelOne", "VeloX", "VeloY", "??VeloZ??" };
	double speedDefault[3] = { 0, 0, 0 };
	double speed[3] = { 0, 0, 0 };
	short angle[3] = { 0, 0, 0 };
	bool resetdefault = true;


    while(!quit) {
        //------------------------------------------------------------------------------------------------------------------
        // Connect 
        device->connect();
        PrintDeviceStatus("Connecting", device);
        device->abortOrbBasicProgram();

        //------------------------------------------------------------------------------------------------------------------
        // Send/Receive Data
        for(; device->state() == SpheroState_Connected;) {
            std::vector<SpheroMessage> messages = device->receive();
            for(auto message : messages) {

				if (message.responseCode == ResponseCode_OK) {
					if (message.idCode == AsyncResponseId_SensorDataStreaming) {
						// Get the angles from dataStream
						
						for (unsigned short i = 0; i < 3; i++) {
							byte high = message.data[(i + 13) * 2];
							byte low  = message.data[(i + 13) * 2 + 1];
							short val = ((high << 8) + low);
							angle[i] = val;
							//std::cout << "[" << parameters[i] << ":" << std::dec << val << "]";
						}

						// Get the accelerometerFiltered from the datastream and kind of get the velocity
						for (unsigned short i = 0; i < 3; i++) {
							byte high = message.data[(i + 16) * 2];
							byte low  = message.data[(i + 16) * 2 + 1];
							short val = ((high << 8) + low);
							//std::cout << "[" << parameters[i] << ":" << std::dec << val << "]";


							if (resetdefault) { 
								speedDefault[i] = val; 
							}
							else {
								//speedDefault[i] = (speedDefault[i]*0.9 - speed[i]*0.1);
								speed[i] *= 0.6;
								speed[i] += (double)(val - speedDefault[i]) * 0.05;
							}
						}

						if (!resetdefault) {
							std::cout.precision(2);
							std::cout << "[vX: " << setw(6) << std::dec << speed[0] << "]";
							std::cout << "[vY: " << setw(6) << std::dec << speed[1] << "]";
							std::cout << "[vZ: " << setw(6) << std::dec << speed[2] << "]";
						}
						resetdefault = false;

						std::cout << std::endl;

					}
					else {
						std::cout << "[Print] ";
						unsigned short i = 0;
						for (i = 0; i < message.data.size()/2; i++) {
							// concatenate 2 bytes to make a short
							byte high = message.data[i * 2];
							byte low = message.data[i * 2 + 1];
							short val = ((high << 8) + low);
							std::cout << std::dec << val << " ";
						}
						std::cout << std::endl;
					}
				}
            }

			// Reconnect 
			if (GetAsyncKeyState('R')) {
				device->connect();
			}
			// Change color (visual test purpose)
			if (GetAsyncKeyState('C')) {
				device->setRGBLedOutput(rand() % 256, rand() % 256, rand() % 256, 0);
			}
			// Show back light
			if (GetAsyncKeyState('A')) {
				device->setBackLEDOutput(0xff);
			}
			// Hide back light
			if (GetAsyncKeyState('Z')) {
				device->setBackLEDOutput(0x00);
			}

			// Set stabilisation off
			if (GetAsyncKeyState('T')) {
				device->setStabilisation(false);
			}

			// Start data stream
			if (GetAsyncKeyState('G')) {
				CommandParameters cp;
				// N (frequency = 400Hz / N)
				cp.push_back(0x0);
				cp.push_back(0xA);
				// M (number of message at the same time; queuing)
				cp.push_back(0x0);
				cp.push_back(0x1);
				// MASK
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				// PCNT (Packet count; 0 for infinite)
				cp.push_back(0x0);
				//MASK2
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);

				device->setDataStreaming(cp);
			}

			//Stop data stream
			if (GetAsyncKeyState('H')) {
				CommandParameters cp;
				// N
				cp.push_back(0x0);
				cp.push_back(0x0);
				// M
				cp.push_back(0x0);
				cp.push_back(0x0);
				// MASK
				cp.push_back(0x0);
				cp.push_back(0x0);
				cp.push_back(0x0);
				cp.push_back(0x0);
				// PCNT
				cp.push_back(0x0);
				//MASK2
				cp.push_back(0x0);
				cp.push_back(0x0);
				cp.push_back(0x0);
				cp.push_back(0x0);

				device->setDataStreaming(cp);
			}


            if(GetAsyncKeyState('Q')) {
                quit = true;
                break;
            }

            Sleep(50);
        }
        PrintDeviceStatus("Poll loop exited", device);
		device->disconnect();

        if(GetAsyncKeyState('Q')) {
            quit = true;
            break;
        }
        Sleep(100);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Disconnect 
    device->disconnect();
    PrintDeviceStatus("Disconnect", device);

    //------------------------------------------------------------------------------------------------------------------
    // Destroy device 
    SpheroRAW_Destroy(device); device = nullptr;

    //------------------------------------------------------------------------------------------------------------------
    // Keep terminal open 
    cin.get();
	return 0;
}

//======================================================================================================================
