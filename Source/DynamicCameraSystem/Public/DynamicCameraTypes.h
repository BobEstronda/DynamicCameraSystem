// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicCameraTypes.generated.h"


class UCurveFloat;
class UWorld;
class AActor;
class APlayerController;
class UDynamicCameraAnchorComponent;


UENUM(BlueprintType)
enum class ECameraBlendType : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	Custom UMETA(DisplayName = "Custom Curve")
};


// Who "owns" the input while a behavior is active.
UENUM(BlueprintType)
enum class ECameraControlMode : uint8
{
	// Camera follows the anchor (first person, shoulder...). Movement input belongs to the character.
	FollowTarget,
	// Behavior owns a free pivot moved by input (top down, orbit).
	FreePivot,
	// Camera sits on the anchor; input only rotates/zooms within limits (fixed camera).
	LimitedLook
};


USTRUCT(BlueprintType)
struct DYNAMICCAMERASYSTEM_API FCameraBlend
{
	GENERATED_BODY()

	// Blend time in seconds. 0 = hard cut
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (ClampMin = "0.0"))
	float Duration = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend")
	ECameraBlendType Type = ECameraBlendType::EaseInOut;

	// Ease exponent (EaseIn / EaseOut / EaseInOut)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (ClampMin = "1.0", ClampMax = "8.0"))
	float Exponent = 2.f;

	// Used when Type == Custom. Curve input and output are both 0..1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (EditCondition = "Type == EArchCameraBlendType::Custom"))
	TObjectPtr<UCurveFloat> Curve = nullptr;

	// Maps normalized time (0..1) to blend alpha (0..1)
	float Evaluate(float NormalizedTime) const;
};


// Everything a behavior needs to know about the outside world for one evaluation. Rebuilt every frame.
struct DYNAMICCAMERASYSTEM_API FCameraContext
{
	TWeakObjectPtr<APlayerController> PlayerController;
	// Actor that owns the active anchor (character, level actor...).
	TWeakObjectPtr<AActor> AnchorOwner;
	// Live world transform of the active anchor component.
	FTransform AnchorTransform = FTransform::Identity;
	UWorld* World = nullptr;
};