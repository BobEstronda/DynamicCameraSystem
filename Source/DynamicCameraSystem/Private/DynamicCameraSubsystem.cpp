// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraSubsystem.h"
#include "DynamicCameraActor.h"
#include "DynamicCameraAnchorComponent.h"
#include "DynamicCameraBehavior.h"
#include "DynamicCameraData.h"
#include "DynamicCameraRegistry.h"

#include "Camera/PlayerCameraManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

void UDynamicCameraSubsystem::Deinitialize()
{
	DeactivateCurrent();

	if (CameraActor)
	{
		CameraActor->Destroy();
		CameraActor = nullptr;
	}

	Super::Deinitialize();
}

UDynamicCameraSubsystem* UDynamicCameraSubsystem::Get(const APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return nullptr;
	}
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		return LocalPlayer->GetSubsystem<UDynamicCameraSubsystem>();
	}
	return nullptr;
}

bool UDynamicCameraSubsystem::SwitchToAnchor(UDynamicCameraAnchorComponent* Anchor)
{
	return Activate(Anchor, nullptr);
}

bool UDynamicCameraSubsystem::SwitchToAnchorByName(FText Anchor)
{
	if (UWorld* World = GetWorld())
	{
		if (UDynamicCameraRegistry* Registry = World->GetSubsystem<UDynamicCameraRegistry>())
		{
			UDynamicCameraAnchorComponent* AC = Registry->GetAnchor(Anchor);
			return Activate(AC, nullptr);
		}
	}
	return false;
}

bool UDynamicCameraSubsystem::SwitchToAnchorWithBlend(UDynamicCameraAnchorComponent* Anchor, const FCameraBlend& Blend)
{
	return Activate(Anchor, &Blend);
}

void UDynamicCameraSubsystem::ReleaseCamera()
{
	DeactivateCurrent();
	bBlending = false;
	bHasLastView = false;

	APlayerController* PC = GetPC();
	if (PC && bHoldingViewTarget)
	{
		PC->bAutoManageActiveCameraTarget = bPreviousAutoManage;

		if (CameraActor && PC->GetViewTarget() == CameraActor)
		{
			AActor* Target = PreviousViewTarget.Get();
			if (!Target || Target == CameraActor)
			{
				Target = PC->GetPawn();
			}
			if (Target)
			{
				PC->SetViewTarget(Target);
			}
		}
	}
	bHoldingViewTarget = false;
	PreviousViewTarget.Reset();

	OnCameraChanged.Broadcast(nullptr, nullptr);
}

void UDynamicCameraSubsystem::AddLookInput(FVector2D Delta)
{
	ActiveBehavior->AddLookInput(Delta);
}

void UDynamicCameraSubsystem::SetMoveInput(FVector2D Axis)
{
	ActiveBehavior->SetMoveInput(Axis);
}

void UDynamicCameraSubsystem::AddZoomInput(float Delta)
{
	ActiveBehavior->AddZoomInput(Delta);
}

bool UDynamicCameraSubsystem::EvaluateCamera(float DeltaTime, FMinimalViewInfo& OutView)
{
	// CalcCamera may be queried more than once per frame; evaluate once.
	if (LastEvalFrame == GFrameCounter && bHasLastView)
	{
		OutView = CachedView;
		return true;
	}

	FCameraContext Context;
	if (!ActiveCameraData || !BuildContext(Context))
	{
		// Anchor was destroyed (level streamed out, actor deleted...). Give the view back next frame,
		// not from inside the camera update.
		if (bHoldingViewTarget)
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
					{
						if (!ActiveAnchor.IsValid())
						{
							ReleaseCamera();
						}
					}));
			}
		}
		return false;
	}

	FMinimalViewInfo Target;
	Target.Location = Context.AnchorTransform.GetLocation();
	Target.Rotation = Context.AnchorTransform.Rotator();
	Target.FOV = ActiveCameraData->FieldOfView;
	if (ActiveCameraData->bApplyPostProcess)
	{
		Target.PostProcessSettings = ActiveCameraData->PostProcess;
		Target.PostProcessBlendWeight = ActiveCameraData->PostProcessWeight;
	}
	else
	{
		Target.PostProcessBlendWeight = 0.f;
	}

	ActiveBehavior->Evaluate(Context, DeltaTime, Target);

	// 3) Blend from the previous pose.
	if (bBlending)
	{
		BlendElapsed += DeltaTime;
		const float Duration = FMath::Max(ActiveBlend.Duration, KINDA_SMALL_NUMBER);
		const float Time = BlendElapsed / Duration;

		if (Time >= 1.f)
		{
			bBlending = false;
			OutView = Target;
		}
		else
		{
			const float Alpha = ActiveBlend.Evaluate(Time);
			OutView = BlendFromView;
			OutView.BlendViewInfo(Target, Alpha);
			// Quaternion slerp avoids odd paths from the engine's linear rotator blend (e.g. FP -> top down).
			OutView.Rotation = FQuat::Slerp(BlendFromView.Rotation.Quaternion(), Target.Rotation.Quaternion(), Alpha).Rotator();
		}
	}
	else
	{
		OutView = Target;
	}

	LastView = OutView;
	CachedView = OutView;
	bHasLastView = true;
	LastEvalFrame = GFrameCounter;
	return true;
}

bool UDynamicCameraSubsystem::Activate(UDynamicCameraAnchorComponent* Anchor, const FCameraBlend* BlendOverride)
{
	if (!IsValid(Anchor) || !Anchor->GetCameraData() || Anchor->GetCameraData() && !Anchor->GetCameraData()->Behavior)
	{
		return false;
	}

	UDynamicCameraData* CameraData = Anchor->GetCameraData();
	if (!CameraData)
	{
		return false;
	}

	APlayerController* PC = GetPC();
	ADynamicCameraActor* Camera = EnsureCameraActor(PC);
	if (!PC || !Camera)
	{
		return false;
	}

	// Snapshot where the player is looking right now, so we can blend away from it.
	FMinimalViewInfo From;
	bool bHaveFrom = false;
	if (bHasLastView)
	{
		From = LastView;
		bHaveFrom = true;
	}
	else if (PC->PlayerCameraManager)
	{
		From = PC->PlayerCameraManager->GetCameraCacheView();
		bHaveFrom = true;
	}

	// Tear down the previous profile.
	DeactivateCurrent();

	ActiveAnchor = Anchor;
	ActiveCameraData = CameraData;

	ActiveBehavior = DuplicateObject<UDynamicCameraBehavior>(ActiveCameraData->Behavior, this);

	FCameraContext Context;
	BuildContext(Context);

	ActiveBehavior->OnActivated(Context);

	SetupInput(PC, ActiveCameraData);

	// Take over the view.
	if (!bHoldingViewTarget)
	{
		PreviousViewTarget = PC->GetViewTarget();
		bPreviousAutoManage = PC->bAutoManageActiveCameraTarget;
		bHoldingViewTarget = true;
	}
	PC->bAutoManageActiveCameraTarget = false; // otherwise (re)possessing a pawn would steal the view back
	if (bHaveFrom && !bHasLastView)
	{
		Camera->SetActorLocationAndRotation(From.Location, From.Rotation);
	}
	if (PC->GetViewTarget() != Camera)
	{
		PC->SetViewTarget(Camera);
	}

	// Blend (done by us, so it works between any two profiles on the same actor).
	const FCameraBlend& Blend = BlendOverride ? *BlendOverride : ActiveCameraData->BlendIn;
	if (bHaveFrom && Blend.Duration > KINDA_SMALL_NUMBER)
	{
		BlendFromView = From;
		ActiveBlend = Blend;
		BlendElapsed = 0.f;
		bBlending = true;
	}
	else
	{
		bBlending = false;
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->SetGameCameraCutThisFrame(); // avoids TAA / Lumen ghosting on hard cuts
		}
	}

	LastEvalFrame = MAX_uint64; // force a fresh evaluation this frame
	OnCameraChanged.Broadcast(Anchor, ActiveCameraData);
	return true;
}

void UDynamicCameraSubsystem::DeactivateCurrent()
{
	if (!ActiveBehavior)
	{
		return;
	}

	ActiveBehavior->OnDeactivated();

	TeardownInput();

	ActiveAnchor.Reset();
	ActiveCameraData = nullptr;
}

ADynamicCameraActor* UDynamicCameraSubsystem::EnsureCameraActor(APlayerController* PC)
{
	if (IsValid(CameraActor))
	{
		return CameraActor;
	}

	UWorld* World = GetWorld();
	if (!World || !PC)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = PC;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CameraActor = World->SpawnActor<ADynamicCameraActor>(ADynamicCameraActor::StaticClass(), FTransform::Identity, Params);
	if (CameraActor)
	{
		CameraActor->Init(this);
#if WITH_EDITOR
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			CameraActor->SetActorLabel(FString::Printf(TEXT("DynamicCamera_Player%d"), LocalPlayer->GetControllerId()));
		}
#endif
	}
	return CameraActor;
}

APlayerController* UDynamicCameraSubsystem::GetPC() const
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	return LocalPlayer ? LocalPlayer->GetPlayerController(GetWorld()) : nullptr;
}

bool UDynamicCameraSubsystem::BuildContext(FCameraContext& OutContext) const
{
	UDynamicCameraAnchorComponent* Anchor = ActiveAnchor.Get();
	if (!Anchor)
	{
		return false;
	}

	//OutContext.Anchor = Anchor;
	OutContext.PlayerController = GetPC();
	OutContext.AnchorOwner = Anchor->GetOwner();
	OutContext.AnchorTransform = Anchor->GetComponentTransform();
	OutContext.World = GetWorld();
	return true;
}

void UDynamicCameraSubsystem::SetupInput(APlayerController* PC, const UDynamicCameraData* CameraData)
{
	if (!PC || !CameraData || !CameraData->bManageInput)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (const UInputMappingContext* Context = CameraData->InputMappingContext.LoadSynchronous())
			{
				InputSubsystem->AddMappingContext(Context, CameraData->InputPriority);
				ActiveMappingContext = Context;
			}
		}
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent);
	if (!EIC)
	{
		return;
	}

	BoundInputComponent = EIC;

	if (CameraData->LookAction)
	{
		BindingHandles.Add(EIC->BindAction(CameraData->LookAction, ETriggerEvent::Triggered, this, &UDynamicCameraSubsystem::HandleLook).GetHandle());
	}
	if (CameraData->MoveAction)
	{
		BindingHandles.Add(EIC->BindAction(CameraData->MoveAction, ETriggerEvent::Triggered, this, &UDynamicCameraSubsystem::HandleMove).GetHandle());
		BindingHandles.Add(EIC->BindAction(CameraData->MoveAction, ETriggerEvent::Completed, this, &UDynamicCameraSubsystem::HandleMoveCompleted).GetHandle());
	}
	if (CameraData->ZoomAction)
	{
		BindingHandles.Add(EIC->BindAction(CameraData->ZoomAction, ETriggerEvent::Triggered, this, &UDynamicCameraSubsystem::HandleZoom).GetHandle());
	}
}

void UDynamicCameraSubsystem::TeardownInput()
{
	if (UEnhancedInputComponent* EIC = BoundInputComponent.Get())
	{
		for (const uint32 Handle : BindingHandles)
		{
			EIC->RemoveBindingByHandle(Handle);
		}
	}
	BindingHandles.Reset();
	BoundInputComponent.Reset();

	if (const UInputMappingContext* Context = ActiveMappingContext.Get())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSubsystem->RemoveMappingContext(Context);
			}
		}
	}
	ActiveMappingContext.Reset();
}

void UDynamicCameraSubsystem::HandleLook(const FInputActionValue& Value)
{
	AddLookInput(Value.Get<FVector2D>());
}

void UDynamicCameraSubsystem::HandleMove(const FInputActionValue& Value)
{
	SetMoveInput(Value.Get<FVector2D>());
}

void UDynamicCameraSubsystem::HandleMoveCompleted(const FInputActionValue& Value)
{
	SetMoveInput(FVector2D::ZeroVector);
}

void UDynamicCameraSubsystem::HandleZoom(const FInputActionValue& Value)
{
	AddZoomInput(Value.Get<float>());
}
