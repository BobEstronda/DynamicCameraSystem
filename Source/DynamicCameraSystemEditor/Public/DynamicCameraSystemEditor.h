// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Editor/UnrealEdEngine.h"

class FDynamicCameraSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void Register(UClass* ComponentClass, const TSharedRef<FComponentVisualizer>& Visualizer);

private:
	TArray<FName> Registered;
};
