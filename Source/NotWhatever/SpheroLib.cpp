// Fill out your copyright notice in the Description page of Project Settings.

#include "NotWhatever.h"
#include "SpheroLib.h"

#include "stdafx.h"
#include <windows.h>
#include "SpheroRAWItf.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

//======================================================================================================================
using namespace std;


void PrintDeviceStatus(string action, ISpheroDevice* device) {
	cout << "Action: " << action << " Result: ";

	if (device == nullptr) {
		cout << "Error - Sphero handle is invalid" << endl;
		return;
	}

	switch (device->state()) {
	case SpheroState_None: { cout << "SpheroRAW not initialized" << endl; break; }
	case SpheroState_Error_BluetoothError: { cout << "Error - Couldn't initialize Bluetooth stack" << endl; break; }
	case SpheroState_Error_BluetoothUnavailable: { cout << "Error - No valid Bluetooth adapter found" << endl; break; }
	case SpheroState_Error_NotPaired: { cout << "Error - Specified Sphero not Paired" << endl; break; }
	case SpheroState_Error_ConnectionFailed: { cout << "Error - Connecting failed" << endl; break; }
	case SpheroState_Disconnected: { cout << "Sphero disconnected" << endl; break; }
	case SpheroState_Connected: { cout << "Sphero connected" << endl; break; }
	}

	cout << endl;
}

void OrbBasicAppendLine(ISpheroDevice* device, std::string lineText) {
	std::vector<ubyte> program(lineText.begin(), lineText.end());
	program.insert(program.end(), '\n');
	device->appendOrbBasicFragment(0, program);
}

//======================================================================================================================

ISpheroDevice *device;

// Sets default values for this component's properties
USpheroLib::USpheroLib()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;


	// Create device 
	device = SpheroRAW_Create("Sphero-GYY");;

	while (!(device->state() == SpheroState_Connected)) {
		// Connect 
		device->connect();
		PrintDeviceStatus("Connecting", device);
		device->abortOrbBasicProgram();
	}

	if (device->state() == SpheroState_Connected) {

		// Show back light
		device->setBackLEDOutput(0xff);

		// Set stabilisation off
		device->setStabilisation(false);

		// Start data stream
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
	else {
		cout << "not connected anymore" << endl;
	}
}


// destructor
USpheroLib::~USpheroLib()
{
	// Disconnect 
	device->disconnect();
	PrintDeviceStatus("Disconnect", device);

	//TODO should we remove backlight/datastream and stabilization ?

	// Destroy device 
	SpheroRAW_Destroy(device); device = nullptr;
}


// Called when the game starts
void USpheroLib::BeginPlay()
{
	Super::BeginPlay();

	// reset sphero orientation
	resetRotationVector();


}


// Called every frame
void USpheroLib::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USpheroLib::updateData()
{
	if (device->state() == SpheroState_Connected) {
		std::vector<SpheroMessage> messages = device->receive();
		for (auto message : messages) {

			if (message.responseCode == ResponseCode_OK) {
				if (message.idCode == AsyncResponseId_SensorDataStreaming) {
					// Get the angles from dataStream

					for (unsigned short i = 0; i < 3; i++) {
						// concatenate 2 bytes to make a short
						unsigned char high = message.data[(i + 13) * 2];
						unsigned char low = message.data[(i + 13) * 2 + 1];
						short val = ((high << 8) + low);
						vecRotation[i] = val;
						//std::cout << "[" << parameters[i] << ":" << std::dec << val << "]";
					}
					std::cout << std::endl;

				}
				else {
					std::cout << "[Print] ";
					unsigned short i = 0;
					for (i = 0; i < message.data.size() / 2; i++) {
						// concatenate 2 bytes to make a short
						unsigned char high = message.data[i * 2];
						unsigned char low = message.data[i * 2 + 1];
						short val = ((high << 8) + low);
						std::cout << std::dec << val << " ";
					}
					std::cout << std::endl;
				}
			}
		}
	}



	// todo get data from sphero
	vecRotation += FVector(1, 0, 0);
}

FVector USpheroLib::getRotationVector()
{
	return vecRotation - vecRotationRelative;
}

FVector USpheroLib::getRotationVectorDelta()
{
	return vecRotation - vecRotationOld;
}

void USpheroLib::resetRotationVector()
{
	updateData(); // todo, good idea to use it here?
	vecRotationRelative = vecRotation;
}

