// Fill out your copyright notice in the Description page of Project Settings.

#include "NotWhatever.h"
#include "SpheroRAWItf.h"


/*class FUE4SpheroGameModule : public FDefaultGameModuleImpl
{
	ISpheroDevice *device;

	//Called right after the module DLL has been loaded and the module object has been created
	
	virtual void StartupModule() override
	{
		device = SpheroRAW_Create("Sphero-GYY");;

	}

	
	//Called before the module is unloaded, right before the module object is destroyed.
	
	virtual void ShutdownModule()
	{
		SpheroRAW_Destroy(device);
		device = NULL;
	}
};*/

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, NotWhatever, "NotWhatever" );
