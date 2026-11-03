// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/CameraBehavior_Default.h"
#include "DynamicCameraMath.h"

void UCameraBehavior_Default::OnActivated(const FCameraContext& Context)
{
	TargetPivotOffset = FVector::ZeroVector;
	CurrentPivotOffset = FVector::ZeroVector;

	CalcDistance();
}

void UCameraBehavior_Default::Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView)
{
	Look(Context, DeltaTime, InOutView.Rotation);
	ApplyZoom(DeltaTime, InOutView);
	Move(Context, DeltaTime, InOutView.Rotation, InOutView.Location);
}

void UCameraBehavior_Default::CalcDistance()
{
	TargetDistance = bZoom ? FMath::Clamp(DefaultDistance, MinDistance, FMath::Max(MinDistance, MaxDistance)) : DefaultDistance;

	CurrentDistance = TargetDistance;
}

void UCameraBehavior_Default::Look(const FCameraContext& Context, const float DeltaTime, FRotator& OutRotation)
{
	FRotator Rotation = Context.AnchorTransform.Rotator();
	if (bUseControlRotation)
	{
		if (const APlayerController* PC = Context.PlayerController.Get())
		{
			Rotation = PC->GetControlRotation();
		}
	}
	else
	{
		const FVector2D Look = ConsumeLookInput();

		TargetYaw = TargetYaw + Look.X * LookSensitivity;
		TargetPitch = TargetPitch + -1.f * Look.Y * LookSensitivity;

		CurrentYaw = DynamicCameraMath::SmoothTo(CurrentYaw, TargetYaw, DeltaTime, LookInterpSpeed);
		CurrentPitch = DynamicCameraMath::SmoothTo(CurrentPitch, TargetPitch, DeltaTime, LookInterpSpeed);

		Rotation = FRotator(Rotation.Pitch + CurrentPitch, Rotation.Yaw + CurrentYaw, 0.f);
	}

	if (YawLimit > 0.f)
	{
		Rotation.Yaw = FMath::Clamp(Rotation.Yaw, -YawLimit, YawLimit);
	}

	if (MinPitch < -0.f || MaxPitch > 1.f)
	{
		Rotation.Pitch = FMath::Clamp(Rotation.Pitch, MinPitch, MaxPitch);
	}

	OutRotation = Rotation;
}

void UCameraBehavior_Default::Move(const FCameraContext& Context, const float DeltaTime, FRotator& ViewRotation, FVector& OutLocation)
{
	if (bMove)
	{
		const FVector2D Move = GetMoveInput();

		if (!Move.IsNearlyZero())
		{
			const float BaseYaw = bUseCameraRotation ? ViewRotation.Yaw : Context.AnchorTransform.Rotator().Yaw;

			const FRotationMatrix YawMatrix(FRotator(0.f, BaseYaw, 0.f));
			const FVector Forward = YawMatrix.GetUnitAxis(EAxis::X);
			const FVector Right = YawMatrix.GetUnitAxis(EAxis::Y);

			const FVector Direction = (Forward * Move.Y + Right * Move.X).GetClampedToMaxSize(1.f);

			FVector NewOffset = TargetPivotOffset + Direction * MoveSpeed * DeltaTime;
			if (MaxMoveRadius > 0.f && NewOffset.SizeSquared() > FMath::Square(MaxMoveRadius))
			{
				NewOffset = NewOffset.GetSafeNormal() * MaxMoveRadius;
			}
			TargetPivotOffset = NewOffset;
		}
	}
	else
	{
		TargetPivotOffset = FVector::ZeroVector;
	}

	CurrentPivotOffset = (MoveInterpSpeed > 0.f)
		? FMath::VInterpTo(CurrentPivotOffset, TargetPivotOffset, DeltaTime, MoveInterpSpeed)
		: TargetPivotOffset;

	const FVector Pivot = Context.AnchorTransform.GetLocation() + CurrentPivotOffset;

	const float BackDistance = bOrthographic ? DefaultDistance : CurrentDistance;

	OutLocation = Pivot - ViewRotation.Vector() * BackDistance;
}

void UCameraBehavior_Default::ApplyZoom(const float DeltaTime, FMinimalViewInfo& InOutView)
{
	if (bZoom)
	{
		const float Zoom = ConsumeZoomInput();

		TargetDistance = FMath::Clamp(
			TargetDistance - Zoom * ZoomSpeed,
			MinDistance,
			FMath::Max(MinDistance, MaxDistance));

		CurrentDistance = (ZoomInterpSpeed > 0.f)
			? DynamicCameraMath::SmoothTo(CurrentDistance, TargetDistance, DeltaTime, ZoomInterpSpeed)
			: TargetDistance;
	}

	if (bOrthographic)
	{
		InOutView.ProjectionMode = ECameraProjectionMode::Orthographic;
		InOutView.OrthoWidth = FMath::Max(CurrentDistance, 1.f);
	}
	else
	{
		InOutView.ProjectionMode = ECameraProjectionMode::Perspective;
	}
}
