// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicCameraAnchorActor.generated.h"

class UDynamicCameraAnchorComponent;

UCLASS()
class DYNAMICCAMERASYSTEM_API ADynamicCameraAnchorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADynamicCameraAnchorActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDynamicCameraAnchorComponent* CameraAnchor = nullptr;
};
