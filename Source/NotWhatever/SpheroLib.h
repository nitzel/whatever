// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "SpheroLib.generated.h"

/**
This class acts as an interface between a Sphero Ball and the Game Objects.
On creation it connects, 

*/
UCLASS()
class NOTWHATEVER_API USpheroLib : public UObject
{
	GENERATED_BODY()
public:
	USpheroLib();
	~USpheroLib();

	/**
	Return pointer to static instance or initiate one if it does not exist yet
	*/
	UFUNCTION(BlueprintCallable, Category = "Sphero")
	static USpheroLib * get();

	/**
	Update the locally stored data about the sphero by reading in the messages
	*/
	UFUNCTION(BlueprintCallable, Category = "Sphero")
	void updateData();

	/**
	Return the rotation-vector of the sphero to the user
	*/
	UFUNCTION(BlueprintPure, Category = "Sphero")
	FVector getRotationVector();

	/**
	Return the rotation-vector of the sphero to the user
	*/
	UFUNCTION(BlueprintPure, Category = "Sphero")
	FVector getRotationVectorDelta();

	/**
	Resets the relative rotation vector to 0.
	*/
	UFUNCTION(BlueprintCallable, Category = "Sphero")
	void resetRotationVector();

protected:
	// class variables
	static class USpheroLib * pSelf;
	// sphero variables
	struct FVector vecRotation; // rotation measured by sphero
	struct FVector vecRotationOld; // rotation measured by sphero before the last updateData call
	struct FVector vecRotationRelative; // initial rotation. difference -> rotation since program start


};

