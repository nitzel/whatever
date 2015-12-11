// Fill out your copyright notice in the Description page of Project Settings.

#include "NotWhatever.h"
#include "SpheroLib.h"


// Sets default values for this component's properties
USpheroLib::USpheroLib()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...


	// TODO init sphero ball here
}


// destructor
USpheroLib::~USpheroLib()
{
	// TODO release sphero ball here
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

