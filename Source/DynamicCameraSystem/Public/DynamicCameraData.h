// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Scene.h"
#include "DynamicCameraTypes.h"
#include "DynamicCameraData.generated.h"


class UDynamicCameraBehavior;
class UInputMappingContext;
class UInputAction;

/**
 *  Data asset that defines HOW a camera behaves: lens, post process, blend, input and the behavior stack.
 *  Anchors only point to a CameraData; the same Data can be shared by any number of anchors.
 */
UCLASS(BlueprintType)
class DYNAMICCAMERASYSTEM_API UDynamicCameraData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Behavior")
	TObjectPtr<UDynamicCameraBehavior> Behavior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lens", meta = (ClampMin = "5", ClampMax = "170"))
	float FieldOfView = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Post Process")
	bool bApplyPostProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Post Process", meta = (EditCondition = "bApplyPostProcess"))
	FPostProcessSettings PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Post Process", meta = (EditCondition = "bApplyPostProcess", ClampMin = "0", ClampMax = "1"))
	float PostProcessWeight = 1.f;

	// Blend used when switching TO this Data.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blend")
	FCameraBlend BlendIn;

	// Input (optional: leave bManageInput off to feed the camera from your own input code)
	// If true, the subsystem adds the mapping context and binds the actions below for the local player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	bool bManageInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "bManageInput"))
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "bManageInput"))
	int32 InputPriority = 10;

	// Axis2D. Value is passed to the behaviors as a look delta.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "bManageInput"))
	TObjectPtr<UInputAction> LookAction;

	// Axis2D (X = right, Y = forward). Held value, reset on Completed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "bManageInput"))
	TObjectPtr<UInputAction> MoveAction;

	// Axis1D.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (EditCondition = "bManageInput"))
	TObjectPtr<UInputAction> ZoomAction;
};
