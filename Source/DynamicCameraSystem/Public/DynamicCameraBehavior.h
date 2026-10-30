// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Camera/CameraTypes.h"
#include "DynamicCameraTypes.h"
#include "DynamicCameraAnchorComponent.h"
#include "DynamicCameraBehavior.generated.h"

/**
 * One piece of camera logic (first person, top down, fixed look...).
 *
 * Authored as an inline instance inside a UDynamicCameraData, so every CameraData can tune its own copy.
 * At runtime the subsystem DUPLICATES the template when a CameraData is activated, so behaviors may keep
 * runtime state (current yaw, pivot...) without touching the data asset.
 *
 * Behaviors run in the order listed in the CameraData; each one reads/writes the same FMinimalViewInfo.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DYNAMICCAMERASYSTEM_API UDynamicCameraBehavior : public UObject
{
	GENERATED_BODY()
	

public:
	virtual ECameraControlMode GetControlMode() const { return ECameraControlMode::FollowTarget; }

	// Called once when the owning profile becomes active. Initialise runtime state from the anchor here.
	virtual void OnActivated(const FCameraContext& Context) {}
	virtual void OnDeactivated() {}

	// Modify the view. Called every frame while active, after the camera actor/character have moved.
	virtual void Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView) {}

	// Point this behavior is looking at / orbiting, if it has one (used to test "camera focus" modifier volumes).
	virtual bool GetFocusPoint(FVector& OutPoint) const { return false; }

	
	//The point this behavior is looking at / orbiting (e.g. the top down pivot), if it has one.
	//Modifier volumes can use it as their reference point ("Focus").
	virtual bool GetFocusLocation(FVector& OutLocation) const { return false; }

	// Input
	// Accumulated look delta (yaw = X, pitch = Y). Consumed by Evaluate.
	void AddLookInput(const FVector2D& Delta) { LookInput += Delta; }

	// Held movement axis (X = right, Y = forward). Persistent until set again.
	void SetMoveInput(const FVector2D& Axis) { MoveInput = Axis; }

	// Accumulated zoom delta. Consumed by Evaluate.
	void AddZoomInput(float Delta) { ZoomInput += Delta; }

protected:
	void RequestSwitch(UDynamicCameraAnchorComponent* Anchor) { PendingSwitch = Anchor; }

	FVector2D ConsumeLookInput()
	{
		const FVector2D Result = LookInput;
		LookInput = FVector2D::ZeroVector;
		return Result;
	}

	float ConsumeZoomInput()
	{
		const float Result = ZoomInput;
		ZoomInput = 0.f;
		return Result;
	}

	const FVector2D& GetMoveInput() const { return MoveInput; }

private:
	FVector2D LookInput = FVector2D::ZeroVector;
	FVector2D MoveInput = FVector2D::ZeroVector;
	float ZoomInput = 0.f;
	TWeakObjectPtr<UDynamicCameraAnchorComponent> PendingSwitch;
};
