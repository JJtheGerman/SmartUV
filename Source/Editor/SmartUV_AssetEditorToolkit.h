#pragma once

#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "UObject/GCObject.h" 

class FSpawnTabArgs;
class IToolkitHost;
class SDockTab;
class USmartUV_Asset;
class SSmartUV_EditorViewport;

namespace ESmartUVEditorMode
{
	enum Type
	{
		ViewMode
	};
}

class FSmartUV_AssetEditorToolkit : public FAssetEditorToolkit, public FEditorUndoClient, public FGCObject
{
public:

	void Initialize(USmartUV_Asset* InSmartMaterialAsset, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost);

	ESmartUVEditorMode::Type GetCurrentMode() const;

public:

	// FAssetEditorToolkit Interface
	virtual FString GetDocumentationLink() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	// FAssetEditorToolkit Interface
	
	// IToolkit Interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// IToolkit Interface

	// FGCObject Interface
	virtual FString GetReferencerName() const override
	{
		return TEXT("FSmartUVEditor");
	}
	// FGCObject Interface

public:

	TSharedPtr<class IDetailsView> GetPropertyView() { return MainPropertyView; };

	// The SmartMaterial being edited
	USmartUV_Asset* SmartUV_Asset;

protected:

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	void ExtendToolbar();
	void CreateModeToolbarWidgets(FToolBarBuilder& IgnoredBuilder);
	void OnPropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent);

private:

	TSharedPtr<SSmartUV_EditorViewport> ViewportPtr;

	// The editors property view
	TSharedPtr<class IDetailsView> MainPropertyView;
};