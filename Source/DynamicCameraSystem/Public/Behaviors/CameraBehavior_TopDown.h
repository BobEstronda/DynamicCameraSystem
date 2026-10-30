// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicCameraBehavior.h"
#include "CameraBehavior_TopDown.generated.h"

/**
 * 
 */
UCLASS()
class DYNAMICCAMERASYSTEM_API UCameraBehavior_TopDown : public UDynamicCameraBehavior
{
	GENERATED_BODY()
	
public:
	// --- Framing ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "1"))
	float DefaultDistance = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "1"))
	float MinDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "1"))
	float MaxDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "-89", ClampMax = "-1"))
	float DefaultPitch = -60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "-89", ClampMax = "-1"))
	float MinPitch = -89.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing", meta = (ClampMin = "-89", ClampMax = "-1"))
	float MaxPitch = -15.f;

	/** Start with the anchor's pitch instead of DefaultPitch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing")
	bool bInitPitchFromAnchor = false;

	/** Orthographic projection (plan views). Note: blending between perspective and ortho is a hard switch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Framing")
	bool bOrthographic = false;

	// --- Pan ---
	/** Units/second at ReferenceDistance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan", meta = (ClampMin = "0"))
	float PanSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan")
	bool bScalePanWithDistance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan", meta = (EditCondition = "bScalePanWithDistance", ClampMin = "1"))
	float ReferenceDistance = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan")
	bool bClampPivot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan", meta = (EditCondition = "bClampPivot"))
	FBox PivotBounds = FBox(ForceInit);

	// --- Rotation ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bAllowRotation = true;

	/** Degrees per input unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "bAllowRotation", ClampMin = "0"))
	float LookSensitivity = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "bAllowRotation"))
	bool bInvertPitch = false;

	// --- Zoom ---
	/** Fraction of the distance removed per zoom input unit (0.1 = 10%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (ClampMin = "0.01", ClampMax = "0.5"))
	float ZoomStep = 0.1f;

	// --- Smoothing (0 = instant) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing", meta = (ClampMin = "0"))
	float PanInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing", meta = (ClampMin = "0"))
	float RotationInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing", meta = (ClampMin = "0"))
	float ZoomInterpSpeed = 10.f;

	// --- Collision (off by default: in archviz the camera usually sits above the ceiling) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bProbeCollision = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (EditCondition = "bProbeCollision", ClampMin = "1"))
	float ProbeRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (EditCondition = "bProbeCollision"))
	TEnumAsByte<ECollisionChannel> ProbeChannel = ECC_Camera;

	virtual ECameraControlMode GetControlMode() const override { return ECameraControlMode::FreePivot; }
	virtual bool GetFocusPoint(FVector& OutPoint) const override { OutPoint = CurrentPivot; return true; }
	virtual void OnActivated(const FCameraContext& Context) override;
	virtual void Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView) override;

private:
	FVector TargetPivot = FVector::ZeroVector;
	FVector CurrentPivot = FVector::ZeroVector;
	float TargetYaw = 0.f;
	float CurrentYaw = 0.f;
	float TargetPitch = -60.f;
	float CurrentPitch = -60.f;
	float TargetDistance = 1500.f;
	float CurrentDistance = 1500.f;
};
