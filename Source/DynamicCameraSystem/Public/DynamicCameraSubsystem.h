// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DynamicCameraTypes.h"
#include "DynamicCameraSubsystem.generated.h"

class ADynamicCameraActor;
class UDynamicCameraAnchorComponent;
class UDynamicCameraBehavior;
class UDynamicCameraData;
class UEnhancedInputComponent;
class UInputMappingContext;
class APlayerController;
struct FInputActionValue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicCameraChangedSignature, UDynamicCameraAnchorComponent*, NewAnchor, UDynamicCameraData*, NewProfile);

/**
 * Per LOCAL PLAYER camera brain (works with split-screen: each player has an independent camera).
 *
 *  - spawns/owns the free camera actor and makes it the player's view target
 *  - activates anchors: instantiates the CameraData's behaviors, sets up input, blends from the previous pose
 *  - evaluates the behavior stack every frame (called from ADynamicCameraActor::CalcCamera)
 */
UCLASS()
class DYNAMICCAMERASYSTEM_API UDynamicCameraSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	

public:
	virtual void Deinitialize() override;

	// Convenience accessor from a player controller.
	UFUNCTION(BlueprintPure, Category = "Camera", meta = (DefaultToSelf = "PlayerController"))
	static UDynamicCameraSubsystem* Get(const APlayerController* PlayerController);

	// Switch using the blend defined by the anchor's profile.
	UFUNCTION(BlueprintCallable, Category = "Camera")
	bool SwitchToAnchor(UDynamicCameraAnchorComponent* Anchor);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	bool SwitchToAnchorByName(FText Anchor);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	bool SwitchToAnchorWithBlend(UDynamicCameraAnchorComponent* Anchor, const FCameraBlend& Blend);

	// Stop controlling the view and give it back to the previous view target (usually the pawn).
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ReleaseCamera();

	UFUNCTION(BlueprintPure, Category = "Camera")
	UDynamicCameraAnchorComponent* GetActiveAnchor() const { return ActiveAnchor.Get(); }

	UFUNCTION(BlueprintPure, Category = "Camera")
	UDynamicCameraData* GetActiveCameraData() const { return ActiveCameraData; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	ADynamicCameraActor* GetCameraActor() const { return CameraActor; }

	UPROPERTY(BlueprintAssignable, Category = "Camera")
	FDynamicCameraChangedSignature OnCameraChanged;

	// Manual input (use these if the profile does not manage input, e.g. you drive it from your own input system)

	UFUNCTION(BlueprintCallable, Category = "Camera|Input")
	void AddLookInput(FVector2D Delta);

	// X = right, Y = forward. Held value: call again with zero to stop.
	UFUNCTION(BlueprintCallable, Category = "Camera|Input")
	void SetMoveInput(FVector2D Axis);

	UFUNCTION(BlueprintCallable, Category = "Camera|Input")
	void AddZoomInput(float Delta);

	// Called by AArchCameraActor::CalcCamera. Returns false when there is nothing active to show.
	bool EvaluateCamera(float DeltaTime, FMinimalViewInfo& OutView);

private:
	bool Activate(UDynamicCameraAnchorComponent* Anchor, const FCameraBlend* BlendOverride);
	void DeactivateCurrent();
	ADynamicCameraActor* EnsureCameraActor(APlayerController* PC);
	APlayerController* GetPC() const;
	bool BuildContext(FCameraContext& OutContext) const;

	void SetupInput(APlayerController* PC, const UDynamicCameraData* CameraData);
	void TeardownInput();
	void HandleLook(const FInputActionValue& Value);
	void HandleMove(const FInputActionValue& Value);
	void HandleMoveCompleted(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);

	UPROPERTY(Transient)
	TObjectPtr<ADynamicCameraActor> CameraActor;

	UPROPERTY(Transient)
	TWeakObjectPtr<UDynamicCameraAnchorComponent> ActiveAnchor;

	UPROPERTY(Transient)
	TObjectPtr<UDynamicCameraData> ActiveCameraData;

	UPROPERTY(Transient)
	TObjectPtr<UDynamicCameraBehavior> ActiveBehavior;

	// View target bookkeeping
	TWeakObjectPtr<AActor> PreviousViewTarget;
	bool bHoldingViewTarget = false;
	bool bPreviousAutoManage = true;

	// Pose / blend state
	FMinimalViewInfo LastView;
	FMinimalViewInfo CachedView;
	FMinimalViewInfo BlendFromView;
	FCameraBlend ActiveBlend;
	float BlendElapsed = 0.f;
	bool bHasLastView = false;
	bool bBlending = false;
	uint64 LastEvalFrame = MAX_uint64;


	// Input bookkeeping
	TWeakObjectPtr<UEnhancedInputComponent> BoundInputComponent;
	TArray<uint32> BindingHandles;
	TWeakObjectPtr<const UInputMappingContext> ActiveMappingContext;
};
