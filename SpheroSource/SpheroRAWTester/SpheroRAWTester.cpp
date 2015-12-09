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

	std::string locator[5] = { "X", "Y", "XS","YS", "SOG" };
	std::string parameters[42] = { "AccX", "AccY", "AccZ", "giroX", "giroY", "giroZ", "reserved", "reserved", "reserved", "rmbEMF", "lmbEMF", "rmPWM", "lmPWM", "IMUpitch", "IMUroll", "IMUyaw", "AXf", "AYf", "AZf", "giroXfilt", "giroYfilt", "giroZfilt", "reserved", "reserved", "reserved", "rmbEMFfilt", "lmbEMFfilt", "reserved", "reserved", "reserved", "reserved", "reserved", "QO", "Q1", "Q2", "Q3", "odoX", "odoY", "AccelOne", "VeloX", "VeloY", "??VeloZ??" };
	short speedXYZdefault[3] = { 0, 0, 0 };
	double speedXYZ[3] = { 0, 0, 0 };
	bool resetdefault = true;


	// If you want to group data together change this value
	unsigned short const CONCATENATED_DATA_NUMBER = 1;
	unsigned short countData = 0;
	MessageData data_store[CONCATENATED_DATA_NUMBER];


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

					// If data size equals 10, it should be the read locator
					if (message.data.size() == 10) {
						if (countData < CONCATENATED_DATA_NUMBER) {
							data_store[countData] = message.data;
							countData++;
						}
						else {
							unsigned short i = 0;
							for (i = 0; i < 5; i++) {
								if (i == 0 || i == 1 || i==4) {
									byte high = data_store[CONCATENATED_DATA_NUMBER-1][i * 2];
									byte low = data_store[CONCATENATED_DATA_NUMBER-1][i * 2 + 1];
									short val = ((high << 8) + low);
									std::cout << "[" << locator[i] << ": " << std::dec << val << "]";
								}
								else {
									unsigned short j = 0;
									short val = 0;
									for (j = 0; j < CONCATENATED_DATA_NUMBER; j++) {
										byte high = message.data[i * 2];
										byte low = message.data[i * 2 + 1];
										val += ((high << 8) + low);
									}
									std::cout << "[" << locator[i] << ": " << std::dec << val << "]";
								}
							}


							std::cout << std::endl;

							countData = 0;													
						}
					}
					else if (message.idCode == AsyncResponseId_SensorDataStreaming) {

						unsigned short i = 0;
						for (i = 16; i < 19; i++) {
							byte high = message.data[i * 2];
							byte low = message.data[i * 2 + 1];
							short val = ((high << 8) + low);
							//std::cout << "[" << parameters[i] << ":" << std::dec << val << "]";


							if (resetdefault) { 
								speedXYZdefault[i - 16] = val; 
							}
							else {
								speedXYZ[i - 16] = (double)(val - speedXYZdefault[i - 16]) * 0.05;
							}
						}

						if (!resetdefault) {
							std::cout << "[vX: " << std::dec << speedXYZ[0] << "]";
							//std::cout << "[vY: " << std::dec << speedXYZ[1] << "]";
							//std::cout << "[vZ: " << std::dec << speedXYZ[2] << "]";
						}
						resetdefault = !resetdefault;

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
			if (GetAsyncKeyState('Z')) {
				device->setBackLEDOutput(0x00);
			}

			if (GetAsyncKeyState('T')) {
				device->setStabilisation(false);
			}

			// Set locator to 0 0
			if (GetAsyncKeyState('S')) {
				CommandParameters cp;
				// FLAGS
				cp.push_back(0x0);
				// X
				cp.push_back(0x0);
				cp.push_back(0x0);
				// Y
				cp.push_back(0x0);
				cp.push_back(0x0);
				// YAW TARE
				cp.push_back(0x0);
				cp.push_back(0x0);

				device->configureLocator(cp);
			}

			// Start data stream
			if (GetAsyncKeyState('G')) {
				CommandParameters cp;
				// N
				cp.push_back(0x0);
				cp.push_back(0xA);
				// M
				cp.push_back(0x0);
				cp.push_back(0x1);
				// MASK
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				cp.push_back(0xFF);
				// PCNT
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

			// Read locator
			//device->readLocator();
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
