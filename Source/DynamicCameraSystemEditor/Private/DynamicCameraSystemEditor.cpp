// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicCameraSystemEditor.h"
#include "UnrealEdGlobals.h"

#include "DynamicCameraAnchorComponent.h"
#include "DynamicCameraSystemEditor/Public/DynamicCameraAnchorVisualizer.h"

#define LOCTEXT_NAMESPACE "FDynamicCameraSystemEditorModule"

void FDynamicCameraSystemEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	Register(UDynamicCameraAnchorComponent::StaticClass(), MakeShared<FDynamicCameraAnchorVisualizer>());
	//Register(UArchCameraSplineAnchorComponent::StaticClass(), MakeShared<FArchCameraAnchorVisualizer>());
	//Register(UArchCameraModifierComponent::StaticClass(), MakeShared<FArchCameraModifierVisualizer>());
}

void FDynamicCameraSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.


	if (GUnrealEd)
	{
		for (const FName& Name : Registered)
		{
			GUnrealEd->UnregisterComponentVisualizer(Name);
		}
	}
	Registered.Reset();
}


void FDynamicCameraSystemEditorModule::Register(UClass* ComponentClass, const TSharedRef<FComponentVisualizer>& Visualizer)
{
	if (GUnrealEd)
	{
		const FName Name = ComponentClass->GetFName();
		GUnrealEd->RegisterComponentVisualizer(Name, Visualizer);
		Visualizer->OnRegister();
		Registered.Add(Name);
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDynamicCameraSystemEditorModule, DynamicCameraSystemEditor)