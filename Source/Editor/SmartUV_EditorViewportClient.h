#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "SEditorViewport.h"

#include "HitProxies.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "Asset/SmartUV_Asset.h"

class FSmartUV_AssetEditorToolkit;
class FCanvas;

class FSelectedItem
{
public:
	int VertexIndex;
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
	// End of HHitProxy interface

	TSharedPtr<FSelectedItem> Data;
	
};

class SelectedIsland
{
public:
	// Constructor
	SelectedIsland(int InSmartUVIndex, FSmartUV_Island InIsland, const FLinearColor& InIslandColor)
		: SmartUVIndex(InSmartUVIndex),
		Island(InIsland),
		IslandLinearColor(InIslandColor)
	{
		
	}

public:

	FORCEINLINE FLinearColor GetIslandColor() const { return IslandLinearColor; }
	
	
public:
	
	// Index of the FUVIsland this class is referencing 
	int SmartUVIndex;

	// If this Island has selection focus
	bool IsSelected;

	// The UVIsland
	FSmartUV_Island Island;

private:
	
	FLinearColor IslandLinearColor;

};

class FSmartUV_EditorViewportClient : public FEditorViewportClient
{
public:
	// Constructor
	explicit FSmartUV_EditorViewportClient(const TWeakPtr<class SEditorViewport>& InEditorViewportWidget = nullptr,
	                                       const TWeakPtr<FSmartUV_AssetEditorToolkit> InToolkit = nullptr);

public:
	// Initialize
	void Initialize();

	// FViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	// End of FViewportClient interface

	// FEditorViewportClient interface
	virtual FLinearColor GetBackgroundColor() const override;
	virtual void CheckHoveredHitProxy(HHitProxy* HoveredHitProxy) override;
	// End of FEditorViewportClient interface

	ESmartUVEditorMode::Type GetCurrentMode() const
	{
		return CurrentMode;
	};

	void UpdatePreviewMaterial(class UTexture* InTexture) const;

	void AddBoxButtonClicked() const;
	
protected:
	
	FIntPoint ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient, FVector InWorldSpaceLocation);
	FSceneView* GetSceneView(class FEditorViewportClient* InViewportClient);

	// Activates the Edit Mode but in our case this will always be active and gets called in the constructor
	void ActivateEditMode() const;

	void DrawUVIslands(FCanvas& Canvas);

private:
	// Current editor mode
	ESmartUVEditorMode::Type CurrentMode;

	// The preview scene
	FPreviewScene OwnedPreviewScene;

	// The preview material used by the mesh plane
	class UMaterial* PreviewMaterial;

	class UStaticMeshComponent* PreviewPlaneMesh;

	// Pointer back to the owning SmartUV editor
	TWeakPtr<FSmartUV_AssetEditorToolkit> SmartUV_EditorPtr;

	class UMaterialInstanceDynamic* DynamicPreviewMaterial;

	// An array of structs so we do not edit the original asset islands directly
	TArray<SelectedIsland> SelectedIslands;
};

class UVIslandRect
{
public:

	// Constructor
	UVIslandRect(FSmartUV_Island InIsland)
	{
		Island = InIsland;
	}

public:
	
	// UV Island in UV space
	FSmartUV_Island Island;

	static TArray<FVector> GetIslandVertices(const FSmartUV_Island& InIsland)
	{
		TArray<FVector> Vertices;
		Vertices.Add(FVector(InIsland.BottomLeft.X, 0, InIsland.BottomLeft.Y)); // BottomLeft
		Vertices.Add(FVector(InIsland.TopRight.X, 0, InIsland.BottomLeft.Y)); // BottomRight
		Vertices.Add(FVector(InIsland.TopRight.X, 0, InIsland.TopRight.Y)); // TopRight
		Vertices.Add(FVector(InIsland.BottomLeft.X, 0, InIsland.TopRight.Y)); // TopLeft

		InvertY(Vertices);

		return Vertices;
	}

	// Visual change to the location of the vertices so they line up with the preview plane in the editor
	static void InvertY(TArray<FVector>& InVertices)
	{
		for (FVector& Vertex : InVertices)
		{
			Vertex.Y *= -1.f;
		}
	}

	FSmartUV_Island ConvertToWSCoords() const
	{
		FSmartUV_Island _iland;
		_iland.BottomLeft = Island.BottomLeft * 100.f;
		_iland.TopRight = Island.TopRight * 100.f;
		return _iland;
	}
};