// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicCameraBehavior.h"
#include "CameraBehavior_Default.generated.h"


UCLASS()
class DYNAMICCAMERASYSTEM_API UCameraBehavior_Default : public UDynamicCameraBehavior
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOrthographic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseControlRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseCameraRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float DefaultDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bZoom", ClampMin = "0"))
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bZoom", ClampMin = "0"))
	float MaxDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "!bUseControlRotation", ClampMin = "-90", ClampMax = "0"))
	float MinPitch = -90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "!bUseControlRotation", ClampMin = "0", ClampMax = "90"))
	float MaxPitch = 90.f;

	/** Max yaw away from the anchor direction, in degrees (each side). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "!bUseControlRotation", ClampMin = "0", ClampMax = "360"))
	float YawLimit = 60.f;

	/** Degrees per input unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "!bUseControlRotation", ClampMin = "0"))
	float LookSensitivity = 0.15f;

	/** 0 = no smoothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (EditCondition = "!bUseControlRotation", ClampMin = "0"))
	float LookInterpSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	bool bMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move", meta = (EditCondition = "bMove", ClampMin = "0"))
	float MoveSpeed = 600.f;

	/** 0 = no smoothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move", meta = (EditCondition = "bMove", ClampMin = "0"))
	float MoveInterpSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move", meta = (EditCondition = "bMove", ClampMin = "0"))
	float MaxMoveRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bZoom = false;

	/** FOV degrees per zoom input unit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bZoom", ClampMin = "0"))
	float ZoomSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bZoom", ClampMin = "0"))
	float ZoomInterpSpeed = 8.f;

	virtual void OnActivated(const FCameraContext& Context) override;
	virtual void Evaluate(const FCameraContext& Context, float DeltaTime, FMinimalViewInfo& InOutView) override;

protected:
	virtual void CalcDistance();
	virtual void Look(const FCameraContext& Context, const float DeltaTime,FRotator& OutRotation);
	virtual void Move(const FCameraContext& Context, const float DeltaTime, FRotator& ViewRotation, FVector& OutLocation);
	virtual void ApplyZoom(const float DeltaTime, FMinimalViewInfo& InOutView);

private:
	FVector TargetPivotOffset = FVector::ZeroVector;
	FVector CurrentPivotOffset = FVector::ZeroVector;
	float TargetYaw = 0.f;
	float CurrentYaw = 0.f;
	float TargetPitch = 0.f;
	float CurrentPitch = 0.f;
	float TargetDistance = 0.f;
	float CurrentDistance = 0.f;
};
