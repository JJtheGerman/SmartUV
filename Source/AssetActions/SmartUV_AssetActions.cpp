#include "AssetActions/SmartUV_AssetActions.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Asset/SmartUV_Asset.h"
#include "Editor/SmartUV_AssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FSmartUV_AssetActions::FSmartUV_AssetActions()
{}

void FSmartUV_AssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* ObjectToEdit : InObjects)
	{
		if (USmartUV_Asset* SmartMaterialAsset = Cast<USmartUV_Asset>(ObjectToEdit))
		{
			TSharedRef<FSmartUV_AssetEditorToolkit> EditorToolkit = MakeShareable(new FSmartUV_AssetEditorToolkit());
			EditorToolkit->Initialize(SmartMaterialAsset, EToolkitMode::Standalone, EditWithinLevelEditor);
		}
	}
}

bool FSmartUV_AssetActions::CanFilter()
{
	return true;
}

uint32 FSmartUV_AssetActions::GetCategories()
{
	return EAssetTypeCategories::Misc | EAssetTypeCategories::MaterialsAndTextures;
}

FText FSmartUV_AssetActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SmartUV", "SmartUV Asset");
}

UClass* FSmartUV_AssetActions::GetSupportedClass() const
{
	return USmartUV_Asset::StaticClass();
}

FColor FSmartUV_AssetActions::GetTypeColor() const
{
	return FColor::White;
}

bool FSmartUV_AssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

#undef LOCTEXT_NAMESPACE