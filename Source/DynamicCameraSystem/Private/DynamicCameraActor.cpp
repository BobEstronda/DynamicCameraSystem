// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraActor.h"
#include "Camera/CameraComponent.h"
#include "DynamicCameraSubsystem.h"

// Sets default values
ADynamicCameraActor::ADynamicCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Purely local: every machine spawns its own camera for its own local players.
	bReplicates = false;
	bNetLoadOnClient = false;
	SetReplicateMovement(false);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->bUsePawnControlRotation = false;
	RootComponent = CameraComponent;

}

void ADynamicCameraActor::Init(UDynamicCameraSubsystem* InSubsystem)
{
	OwningSubsystem = InSubsystem;
}

void ADynamicCameraActor::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	FMinimalViewInfo View;
	UDynamicCameraSubsystem* Subsystem = OwningSubsystem.Get();

	if (Subsystem && Subsystem->EvaluateCamera(DeltaTime, View))
	{
		// Move the actor too: audio listener, culling and anything attached to the camera follow it.
		SetActorLocationAndRotation(View.Location, View.Rotation);
		CameraComponent->SetFieldOfView(View.FOV);
		OutResult = View;
		return;
	}

	Super::CalcCamera(DeltaTime, OutResult);
}
