// Fill out your copyright notice in the Description page of Project Settings.

#include "NotWhatever.h"
#include "SpheroLib.h"

// redefine & init static variables
USpheroLib * USpheroLib::pSelf = nullptr;
//FVector USpheroLib::vecRotation = FVector(0, 0, 0); 
//FVector USpheroLib::vecRotationOld = FVector(0,0,0);
//FVector USpheroLib::vecRotationRelative = FVector(0, 0, 0); 

USpheroLib::USpheroLib() 
{
	// todo connect to sphero
}
USpheroLib::~USpheroLib() 
{
	// todo disconnect from sphero
}

USpheroLib * USpheroLib::get() 
{
	if (pSelf == nullptr) // ptr not set yet
		pSelf = new USpheroLib(); // created it
	return pSelf; // return ptr
}

void USpheroLib::updateData()
{
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