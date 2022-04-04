// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSmartUV_Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();
	virtual bool SupportsDynamicReloading() override;

public:

	void RegisterAssetTypeAction(class IAssetTools& AssetTools, TSharedRef<class IAssetTypeActions> Action);

	/** Holds the plug-ins style set. */
	TSharedPtr<class ISlateStyle> Style;
};