// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DynamicCameraTypes.h"
#include "DynamicCameraAnchorComponent.generated.h"

class UDynamicCameraData;
class UCameraComponent;
class UWorld;
class AActor;

/**
 * The "target" of the camera. Add it to ANY actor (character head socket, a prop, a plain empty actor in the level). It is NOT a camera: it only stores a transform (its own world transform,
 * so you can attach it to a head socket, a spring-arm-like offset, or leave it floating in the level)
 * and which CameraData should interpret it.
 *
 * Registers itself in the world registry on BeginPlay so the camera subsystem can find it.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DYNAMICCAMERASYSTEM_API UDynamicCameraAnchorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UDynamicCameraAnchorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UDynamicCameraData> CameraData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FText AnchorName;

	// What this anchor would show: preview pose. Used by the PIP preview and the debug screen.
	bool BuildPreviewView(FMinimalViewInfo& OutView) const;

#if WITH_EDITOR
	// Feeds the editor's picture-in-picture preview (same mechanism as camera components). Select the component to see it.
	virtual bool GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintPure, Category = "Camera")
	UDynamicCameraData* GetCameraData() const { return CameraData; }

	// Poses worth drawing in the editor / debug. The base anchor returns its own transform; spline anchors return samples along the path.
	virtual void GetPreviewPose(FTransform& OutPose) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
#endif

private:
	bool ShouldShowDebugView() const;

	// Creates / destroys the debug screen
	void RefreshDebugView();

	// Spawned on demand, never saved. Owns the scene capture and the screen mesh.
	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> DebugCameraComponent;
};
