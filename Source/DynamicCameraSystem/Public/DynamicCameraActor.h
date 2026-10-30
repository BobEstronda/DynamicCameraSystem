// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicCameraActor.generated.h"

class UCameraComponent;
class UDynamicCameraSubsystem;

/**
 * The free camera actor. One per local player, spawned locally by UDynamicCameraSubsystem (never replicated).
 * It holds no logic: the subsystem computes the pose and this actor just presents it.
 *
 * The pose is computed inside CalcCamera, which the PlayerCameraManager calls AFTER all gameplay ticking,
 * so the camera never lags behind the character it follows (no jitter, no tick-order fiddling).
 */
UCLASS(NotPlaceable, Transient)
class DYNAMICCAMERASYSTEM_API ADynamicCameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADynamicCameraActor();

	void Init(UDynamicCameraSubsystem* InSubsystem);

	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Dynamic Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	TWeakObjectPtr<UDynamicCameraSubsystem> OwningSubsystem;

};
