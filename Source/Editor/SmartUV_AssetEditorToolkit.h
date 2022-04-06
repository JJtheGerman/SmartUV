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

	FSmartUV_AssetEditorToolkit();
	~FSmartUV_AssetEditorToolkit();

public:

	void Initialize(USmartUV_Asset* InSmartMaterialAsset, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost);

	ESmartUVEditorMode::Type GetCurrentMode() const;

public:

	// FAssetEditorToolkit interface
	virtual FString GetDocumentationLink() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

public:

	// IToolkit interface
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

public:

	TSharedPtr<class IDetailsView> GetPropertyView() { return MainPropertyView; };

	// The SmartMaterial being edited
	USmartUV_Asset* SmartUV_Asset;

protected:

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	void OnPropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent);

private:

	TSharedPtr<SSmartUV_EditorViewport> ViewportPtr;

	// The editors property view
	TSharedPtr<class IDetailsView> MainPropertyView;
};