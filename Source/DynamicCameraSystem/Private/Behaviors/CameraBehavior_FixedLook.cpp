// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/CameraBehavior_FixedLook.h"
#include "DynamicCameraMath.h"

void UCameraBehavior_FixedLook::OnActivated(const FCameraContext& Context)
{
	TargetYaw = CurrentYaw = 0.f;
	TargetPitch = CurrentPitch = 0.f;
	bFOVInitialised = false;
}

void UCameraBehavior_FixedLook::Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView)
{
	// --- Look ---
	const FVector2D Look = ConsumeLookInput();
	if (bAllowLook)
	{
		TargetYaw = FMath::Clamp(TargetYaw + Look.X * LookSensitivity, -YawLimit, YawLimit);
		const float PitchSign = bInvertPitch ? -1.f : 1.f;
		TargetPitch = FMath::Clamp(TargetPitch + PitchSign * Look.Y * LookSensitivity, PitchMin, PitchMax);
	}
	CurrentYaw = DynamicCameraMath::SmoothTo(CurrentYaw, TargetYaw, DeltaTime, LookInterpSpeed);
	CurrentPitch = DynamicCameraMath::SmoothTo(CurrentPitch, TargetPitch, DeltaTime, LookInterpSpeed);

	const FRotator Base = Context.AnchorTransform.Rotator();
	InOutView.Location = Context.AnchorTransform.GetLocation();
	InOutView.Rotation = FRotator(
		Base.Pitch + CurrentPitch,
		Base.Yaw + CurrentYaw,
		Base.Roll);

	// --- Zoom (FOV) ---
	const float Zoom = ConsumeZoomInput();
	if (bAllowZoom)
	{
		if (!bFOVInitialised)
		{
			TargetFOV = CurrentFOV = FMath::Clamp(InOutView.FOV, MinFOV, MaxFOV);
			bFOVInitialised = true;
		}
		TargetFOV = FMath::Clamp(TargetFOV - Zoom * ZoomSpeed, MinFOV, MaxFOV);
		CurrentFOV = DynamicCameraMath::SmoothTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);
		InOutView.FOV = CurrentFOV;
	}
}
