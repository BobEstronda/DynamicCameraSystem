// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicCameraBehavior.h"
#include "CameraBehavior_FirstPerson.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICCAMERASYSTEM_API UCameraBehavior_FirstPerson : public UDynamicCameraBehavior
{
	GENERATED_BODY()
	

public:
	/** Offset applied in the anchor's local space (e.g. slightly in front of the eyes). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	FVector LocalOffset = FVector::ZeroVector;

	/** Use the player controller rotation. If false, uses the anchor's own rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	bool bUseControlRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
	bool bLockRoll = true;

	virtual ECameraControlMode GetControlMode() const override { return ECameraControlMode::FollowTarget; }
	virtual void Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView) override;

};
