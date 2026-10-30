// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace DynamicCameraMath
{
	/** Smooth towards Target. Speed <= 0 snaps instantly. */
	FORCEINLINE float SmoothTo(float Current, float Target, float DeltaTime, float Speed)
	{
		return Speed > 0.f ? FMath::FInterpTo(Current, Target, DeltaTime, Speed) : Target;
	}

	FORCEINLINE FVector SmoothTo(const FVector& Current, const FVector& Target, float DeltaTime, float Speed)
	{
		return Speed > 0.f ? FMath::VInterpTo(Current, Target, DeltaTime, Speed) : Target;
	}
}