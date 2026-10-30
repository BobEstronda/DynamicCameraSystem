// Fill out your copyright notice in the Description page of Project Settings.


#include "Behaviors/CameraBehavior_FirstPerson.h"
//#include "DynamicCameraMath.h"

void UCameraBehavior_FirstPerson::Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView)
{
	InOutView.Location = Context.AnchorTransform.TransformPositionNoScale(LocalOffset);

	FRotator Rotation = Context.AnchorTransform.Rotator();
	if (bUseControlRotation)
	{
		if (const APlayerController* PC = Context.PlayerController.Get())
		{
			Rotation = PC->GetControlRotation();
		}
	}

	if (bLockRoll)
	{
		Rotation.Roll = 0.f;
	}
	InOutView.Rotation = Rotation;
}
