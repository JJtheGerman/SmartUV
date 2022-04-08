#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "SEditorViewport.h"

class FCanvas;

class FSmartUV_EditorViewportClient : public FEditorViewportClient
{
public:

	/** Constructor */
	explicit FSmartUV_EditorViewportClient(const TWeakPtr<class SEditorViewport>& InEditorViewportWidget = nullptr);

	// FViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	// End of FViewportClient interface

	// FEditorViewportClient interface
	virtual FLinearColor GetBackgroundColor() const override;
	// End of FEditorViewportClient interface

	ESmartUVEditorMode::Type GetCurrentMode()
	{
		return CurrentMode;
	};

	void UpdatePreviewMaterial(class UTexture* InTexture);

	void AddBoxButtonClicked();

protected:

	void DrawSelectionRectangles(FViewport* Viewport, FCanvas* Canvas);

	FIntPoint ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient, FVector InWorldSpaceLocation, bool InClampValues);
	FSceneView* GetSceneView(class FEditorViewportClient* InViewportClient);

private:

	// Current editor mode
	ESmartUVEditorMode::Type CurrentMode;

	// The preview scene
	FPreviewScene OwnedPreviewScene;

	// The preview material used by the mesh plane
	class UMaterial* PreviewMaterial;

	class UStaticMeshComponent* PreviewPlaneMesh;

	// Pointer back to the owning SmartUV editor
	TSharedPtr<FSmartUV_AssetEditorToolkit> SmartUV_EditorPtr;

	class UMaterialInstanceDynamic* DynamicPreviewMaterial;
};
