// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmartUV_Module.h"

#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "IAssetTools.h"

#include "AssetActions/SmartUV_AssetActions.h"
#include "Asset/SmartUV_Asset.h"

#include "Editor/SmartUV_EditMode.h"

#define LOCTEXT_NAMESPACE "FSmartUV_Module"

void FSmartUV_Module::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FSmartUV_Module::OnPostEngineInit);
}

void FSmartUV_Module::OnPostEngineInit()
{
	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FSmartUV_AssetActions()));

	SmartUVEditor_ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager());
}

void FSmartUV_Module::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
}

bool FSmartUV_Module::SupportsDynamicReloading()
{
	return true;
}

void FSmartUV_Module::ShutdownModule()
{
	SmartUVEditor_ToolBarExtensibilityManager.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSmartUV_Module, SmartUV)