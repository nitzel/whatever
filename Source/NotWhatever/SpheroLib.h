// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpheroLib.generated.h"

/**
 * 
 */
UCLASS()
class NOTWHATEVER_API USpheroLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	USpheroLib();
	~USpheroLib();
	
	static UFUNCTION(BlueprintCallable, Category = "Sphero")
	void updateData();

	static UFUNCTION(BlueprintCallable, Category = "Sphero")
	FVector getRotationVector();

	static UFUNCTION(BlueprintCallable, Category = "Sphero")
	int schnappi(int a) { return 7 * a; };

	static struct FVector vecRotation; // rotation measured by sphero
	static struct FVector vecRotationRelative; // initial rotation. difference -> rotation since program start

	
};
