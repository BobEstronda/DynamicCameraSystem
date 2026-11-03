// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraAnchorActor.h"

#include "DynamicCameraAnchorComponent.h"

// Sets default values
ADynamicCameraAnchorActor::ADynamicCameraAnchorActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	CameraAnchor = CreateDefaultSubobject<UDynamicCameraAnchorComponent>("CameraAnchor");
	CameraAnchor->SetupAttachment(GetRootComponent());

}



