// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraAnchorComponent.h"
#include "DynamicCameraRegistry.h"
#include "DynamicCameraData.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "Editor.h"
#include "LevelEditorViewport.h"
#include "ScopedTransaction.h"
#endif

// Sets default values for this component's properties
UDynamicCameraAnchorComponent::UDynamicCameraAnchorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


bool UDynamicCameraAnchorComponent::BuildPreviewView(FMinimalViewInfo& OutView) const
{
	const FTransform Pose = FTransform(GetComponentQuat(), GetComponentLocation());
	OutView.Location = Pose.GetLocation();
	OutView.Rotation = Pose.Rotator();
	OutView.FOV = CameraData ? CameraData->FieldOfView : 90.f;
	OutView.bConstrainAspectRatio = true;

	return true;
}

#if WITH_EDITOR
bool UDynamicCameraAnchorComponent::GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut)
{
	return BuildPreviewView(ViewOut);
}

void UDynamicCameraAnchorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RefreshDebugView();
}
#endif

void UDynamicCameraAnchorComponent::GetPreviewPose(FTransform& OutPose) const
{
	OutPose = FTransform(GetComponentQuat(), GetComponentLocation());
}

// Called when the game starts
void UDynamicCameraAnchorComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World && World->GetNetMode() != NM_DedicatedServer)
	{
		if (UDynamicCameraRegistry* Registry = World->GetSubsystem<UDynamicCameraRegistry>())
		{
			Registry->RegisterAnchor(this);
		}
	}
	
}

void UDynamicCameraAnchorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UDynamicCameraRegistry* Registry = World->GetSubsystem<UDynamicCameraRegistry>())
		{
			Registry->UnregisterAnchor(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UDynamicCameraAnchorComponent::OnRegister()
{
	Super::OnRegister();
	RefreshDebugView();
}

void UDynamicCameraAnchorComponent::OnUnregister()
{
	if (DebugCameraComponent)
	{
		DebugCameraComponent->DestroyComponent();
		DebugCameraComponent = nullptr;
	}
	Super::OnUnregister();
}
#endif

bool UDynamicCameraAnchorComponent::ShouldShowDebugView() const
{
	const UWorld* World = GetWorld();
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return false;
	}

	const bool bEditorWorld = World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview;
	return bEditorWorld;
}

void UDynamicCameraAnchorComponent::RefreshDebugView()
{
	const bool bWanted = ShouldShowDebugView();

	if (bWanted && !IsValid(DebugCameraComponent))
	{
		AActor* Owner = GetOwner();
		if (!Owner || !IsRegistered())
		{
			return;
		}

		UCameraComponent* Helper = NewObject<UCameraComponent>(Owner, NAME_None, RF_Transient);
		Helper->CreationMethod = EComponentCreationMethod::Instance;
		Helper->SetupAttachment(this);
		Helper->RegisterComponent();
		DebugCameraComponent = Helper;
	}
	else if (!bWanted && DebugCameraComponent)
	{
		DebugCameraComponent->DestroyComponent();
		DebugCameraComponent = nullptr;
	}
}

