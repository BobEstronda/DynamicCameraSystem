// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"


class UActorComponent;


// Draws the camera frustum(s) of an anchor: one for a plain anchor, several along the path for a spline anchor.
class FDynamicCameraAnchorVisualizer : public FComponentVisualizer
{
public:
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};


/**
 * 
 */
//class DYNAMICCAMERASYSTEMEDITOR_API DynamicCameraAnchorVisualizer
//{
//public:
//	DynamicCameraAnchorVisualizer();
//	~DynamicCameraAnchorVisualizer();
//};
