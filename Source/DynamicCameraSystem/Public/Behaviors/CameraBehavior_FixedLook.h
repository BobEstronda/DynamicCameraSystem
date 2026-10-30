// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicCameraBehavior.h"
#include "CameraBehavior_FixedLook.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICCAMERASYSTEM_API UCameraBehavior_FixedLook : public UDynamicCameraBehavior
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	bool bAllowLook = true;

	/** Max yaw away from the anchor direction, in degrees (each side). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "bAllowLook", ClampMin = "0", ClampMax = "180"))
	float YawLimit = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "bAllowLook", ClampMin = "-89", ClampMax = "0"))
	float PitchMin = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "bAllowLook", ClampMin = "0", ClampMax = "89"))
	float PitchMax = 30.f;

	/** Degrees per input unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "bAllowLook", ClampMin = "0"))
	float LookSensitivity = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "bAllowLook"))
	bool bInvertPitch = false;

	/** 0 = no smoothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (ClampMin = "0"))
	float LookInterpSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bAllowZoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bAllowZoom", ClampMin = "5", ClampMax = "170"))
	float MinFOV = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bAllowZoom", ClampMin = "5", ClampMax = "170"))
	float MaxFOV = 90.f;

	/** FOV degrees per zoom input unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bAllowZoom", ClampMin = "0"))
	float ZoomSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bAllowZoom", ClampMin = "0"))
	float ZoomInterpSpeed = 8.f;

	virtual ECameraControlMode GetControlMode() const override { return ECameraControlMode::LimitedLook; }
	virtual void OnActivated(const FCameraContext& Context) override;
	virtual void Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView) override;

private:
	float TargetYaw = 0.f;
	float CurrentYaw = 0.f;
	float TargetPitch = 0.f;
	float CurrentPitch = 0.f;
	float TargetFOV = 90.f;
	float CurrentFOV = 90.f;
	bool bFOVInitialised = false;
};
