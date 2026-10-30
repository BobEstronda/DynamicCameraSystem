// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraAnchorVisualizer.h"
#include "DynamicCameraAnchorComponent.h"
#include "DynamicCameraData.h"
#include "SceneManagement.h"

namespace
{
	void DrawFrustum(FPrimitiveDrawInterface* PDI, const FTransform& Pose, float FOV, float Distance, const FLinearColor& Color)
	{
		constexpr float AspectRatio = 16.f / 9.f;
		const float HalfWidth = Distance * FMath::Tan(FMath::DegreesToRadians(FOV * 0.5f));
		const float HalfHeight = HalfWidth / AspectRatio;

		const FVector Origin = Pose.GetLocation();
		const FVector Corners[4] =
		{
			Pose.TransformPosition(FVector(Distance, -HalfWidth, HalfHeight)),
			Pose.TransformPosition(FVector(Distance, HalfWidth, HalfHeight)),
			Pose.TransformPosition(FVector(Distance, HalfWidth, -HalfHeight)),
			Pose.TransformPosition(FVector(Distance, -HalfWidth, -HalfHeight))
		};

		for (int32 Index = 0; Index < 4; ++Index)
		{
			PDI->DrawLine(Origin, Corners[Index], Color, SDPG_Foreground, 1.f);
			PDI->DrawLine(Corners[Index], Corners[(Index + 1) % 4], Color, SDPG_Foreground, 1.f);
		}

		// "Up" marker so the roll of the frame is readable.
		const FVector TopMid = Pose.TransformPosition(FVector(Distance, 0.f, HalfHeight * 1.25f));
		PDI->DrawLine((Corners[0] + Corners[1]) * 0.5f, TopMid, Color, SDPG_Foreground, 1.f);
	}
}


void FDynamicCameraAnchorVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const UDynamicCameraAnchorComponent* Anchor = Cast<UDynamicCameraAnchorComponent>(Component);
	if (!Anchor || !PDI)
	{
		return;
	}

	const UDynamicCameraData* CameraData = Anchor->GetCameraData();
	const float FOV = CameraData ? CameraData->FieldOfView : 90.f;

	FTransform Pose;
	Anchor->GetPreviewPose(Pose);

	const FLinearColor Color(1.f, 0.7f, 0.f);

	DrawFrustum(PDI, Pose, FOV, 150.f, Color);
}