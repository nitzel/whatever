// Fill out your copyright notice in the Description page of Project Settings.

#include "NotWhatever.h"
#include "SpheroLib.h"

FVector USpheroLib::vecRotation = FVector(0,0,0); // rotation measured by sphero
FVector USpheroLib::vecRotationRelative = FVector(0, 0, 0);; // initial rotation. difference -> rotation since program start

USpheroLib::USpheroLib()
{
	// TODO: init here
	// connect to sphero

	vecRotationRelative = FVector(0, 0, 0);
}

USpheroLib::~USpheroLib()
{
	// TODO: disconnect/ uninitialize
}
void USpheroLib::updateData()
{
	vecRotation += FVector(1, 0, 0);
}

FVector USpheroLib::getRotationVector()
{
	return vecRotation - vecRotationRelative;
}

