// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define SMARTUV_EDITOR_MODULE_NAME "SmartUV"

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

	virtual TSharedPtr<class FExtensibilityManager> GetSmartUVEditorMenuExtensibilityManager() { return SmartUVEditor_ToolBarExtensibilityManager; }
	TSharedPtr<FExtensibilityManager> SmartUVEditor_ToolBarExtensibilityManager;
};