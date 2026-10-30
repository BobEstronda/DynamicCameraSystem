// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/CameraBehavior_TopDown.h"
#include "DynamicCameraMath.h"


void UCameraBehavior_TopDown::OnActivated(const FCameraContext& Context)
{
	const FVector AnchorLocation = Context.AnchorTransform.GetLocation();
	const FRotator AnchorRotation = Context.AnchorTransform.Rotator();

	TargetYaw = CurrentYaw = AnchorRotation.Yaw;

	const float StartPitch = bInitPitchFromAnchor ? FRotator::NormalizeAxis(AnchorRotation.Pitch) : DefaultPitch;
	TargetPitch = CurrentPitch = FMath::Clamp(StartPitch, MinPitch, MaxPitch);

	TargetDistance = CurrentDistance = FMath::Clamp(DefaultDistance, MinDistance, MaxDistance);

	// Place the pivot so that the first frame sits exactly on the anchor.
	const FVector Forward = FRotator(CurrentPitch, CurrentYaw, 0.f).Vector();
	TargetPivot = CurrentPivot = AnchorLocation + Forward * CurrentDistance;
}

void UCameraBehavior_TopDown::Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView)
{
	const FVector2D Look = ConsumeLookInput();
	const float Zoom = ConsumeZoomInput();
	const FVector2D Move = GetMoveInput();

	// --- Rotation ---
	if (bAllowRotation)
	{
		const float PitchSign = bInvertPitch ? -1.f : 1.f;
		TargetYaw += Look.X * LookSensitivity;
		TargetPitch = FMath::Clamp(TargetPitch + PitchSign * Look.Y * LookSensitivity, MinPitch, MaxPitch);
	}

	// --- Zoom ---
	if (!FMath::IsNearlyZero(Zoom))
	{
		TargetDistance = FMath::Clamp(TargetDistance * FMath::Pow(1.f - ZoomStep, Zoom), MinDistance, MaxDistance);
	}

	// --- Pan (relative to what the screen shows, so it follows the current yaw) ---
	if (!Move.IsNearlyZero())
	{
		const FRotationMatrix YawMatrix(FRotator(0.f, CurrentYaw, 0.f));
		const FVector Forward = YawMatrix.GetUnitAxis(EAxis::X);
		const FVector Right = YawMatrix.GetUnitAxis(EAxis::Y);
		const float Scale = bScalePanWithDistance ? (CurrentDistance / FMath::Max(ReferenceDistance, 1.f)) : 1.f;

		TargetPivot += (Forward * Move.Y + Right * Move.X) * (PanSpeed * Scale * DeltaTime);

		if (bClampPivot && PivotBounds.IsValid)
		{
			TargetPivot = PivotBounds.GetClosestPointTo(TargetPivot);
		}
	}

	// --- Smoothing ---
	CurrentPivot = DynamicCameraMath::SmoothTo(CurrentPivot, TargetPivot, DeltaTime, PanInterpSpeed);
	CurrentYaw = DynamicCameraMath::SmoothTo(CurrentYaw, TargetYaw, DeltaTime, RotationInterpSpeed);
	CurrentPitch = DynamicCameraMath::SmoothTo(CurrentPitch, TargetPitch, DeltaTime, RotationInterpSpeed);
	CurrentDistance = DynamicCameraMath::SmoothTo(CurrentDistance, TargetDistance, DeltaTime, ZoomInterpSpeed);

	// --- Final pose ---
	// Modifier volumes can push the distance / angles without touching the user's own input state.
	const float Distance = CurrentDistance;
	const FRotator Rotation(
		FMath::Clamp(CurrentPitch, -89.f, 89.f),
		CurrentYaw,
		0.f);
	FVector Location = CurrentPivot - Rotation.Vector() * Distance;

	if (bProbeCollision && Context.World)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(ArchCameraProbe), false);
		if (const APlayerController* PC = Context.PlayerController.Get())
		{
			if (const APawn* Pawn = PC->GetPawn())
			{
				Params.AddIgnoredActor(Pawn);
			}
		}

		FHitResult Hit;
		if (Context.World->SweepSingleByChannel(Hit, CurrentPivot, Location, FQuat::Identity, ProbeChannel,
			FCollisionShape::MakeSphere(ProbeRadius), Params))
		{
			Location = Hit.Location;
		}
	}

	InOutView.Location = Location;
	InOutView.Rotation = Rotation;

	if (bOrthographic)
	{
		InOutView.ProjectionMode = ECameraProjectionMode::Orthographic;
		InOutView.OrthoWidth = 2.f * Distance * FMath::Tan(FMath::DegreesToRadians(InOutView.FOV * 0.5f));
	}
}
