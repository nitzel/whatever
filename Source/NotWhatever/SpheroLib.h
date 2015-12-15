// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SpheroRAWItf.h"
#include "SpheroLib.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NOTWHATEVER_API USpheroLib : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USpheroLib();
	~USpheroLib();

	// Called when the game starts
	virtual void BeginPlay() override;

	/*Called from AActor::EndPlay only if bHasBegunPlay is true
		*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


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

	//protected:
	// sphero variables
	struct FVector vecRotation; // rotation measured by sphero
	struct FVector vecRotationOld; // rotation measured by sphero before the last updateData call
	struct FVector vecRotationRelative; // initial rotation. difference -> rotation since program start

	ISpheroDevice *device;


};
