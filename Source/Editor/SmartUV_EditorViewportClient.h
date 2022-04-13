#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "SEditorViewport.h"

#include "HitProxies.h"

class FCanvas;

class FSmartUV_EditorViewportClient : public FEditorViewportClient
{
public:

	/** Constructor */
	explicit FSmartUV_EditorViewportClient(const TWeakPtr<class SEditorViewport>& InEditorViewportWidget = nullptr, const TSharedPtr<FSmartUV_AssetEditorToolkit> InToolkit = nullptr);

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

	FIntPoint ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient, FVector InWorldSpaceLocation);
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

class UVIslandRect
{
public:

	// UV Island in UV space
	FSmartUV_Island Island;

public:

	UVIslandRect(FSmartUV_Island InIsland)
	{
		Island = InIsland;
	}

	TArray<FVector> GetIslandVertices(const FSmartUV_Island& InIsland)
	{
		TArray<FVector> Vertices;
		Vertices.Add(FVector(InIsland.BottomLeft.X, InIsland.BottomLeft.Y, 0.f)); // BottomLeft
		Vertices.Add(FVector(InIsland.TopRight.X, InIsland.BottomLeft.Y, 0.f));   // BottomRight
		Vertices.Add(FVector(InIsland.TopRight.X, InIsland.TopRight.Y, 0.f));     // TopRight
		Vertices.Add(FVector(InIsland.BottomLeft.X, InIsland.TopRight.Y, 0.f));	  // TopLeft

		InvertY(Vertices);

		return Vertices;
	}

	// Visual change to the location of the vertices so they line up with the preview plane in the editor
	void InvertY(TArray<FVector>& InVertices)
	{
		for (FVector& Vertex : InVertices)
		{
			Vertex.Y *= -1.f;
		}
	}

	FSmartUV_Island ConvertToWSCoords()
	{
		FSmartUV_Island _iland = FSmartUV_Island();
		_iland.BottomLeft = Island.BottomLeft * 100.f;
		_iland.TopRight = Island.TopRight * 100.f;
		return _iland;
	}
};

class FSelectedItem
{
public:
		
	int VertexIndex;

public:

	virtual EMouseCursor::Type GetMouseCursor() const
	{
		return EMouseCursor::Default;
	}
};

struct HIslandSelectableObjectHitProxy : public HHitProxy
{
	DECLARE_HIT_PROXY(SMARTUV_API);

	HIslandSelectableObjectHitProxy(TSharedPtr<FSelectedItem> InData)
		: HHitProxy(HPP_World)
		, Data(InData)
	{
	}

	// HHitProxy interface
	virtual bool AlwaysAllowsTranslucentPrimitives() const override { return true; }
	virtual EMouseCursor::Type GetMouseCursor() override { return Data->GetMouseCursor(); }
	// End of HHitProxy interface

	TSharedPtr<FSelectedItem> Data;
};
