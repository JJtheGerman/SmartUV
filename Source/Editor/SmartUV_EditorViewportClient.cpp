#include "SmartUV_EditorViewportClient.h"
#include "AssetEditorModeManager.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Asset/SmartUV_Asset.h"
#include "CanvasItem.h"
#include "SmartUV_EditMode.h"
#include "UnrealWidget.h"
#include "Utils.h"

IMPLEMENT_HIT_PROXY(HIslandSelectableObjectHitProxy, HHitProxy)

FSmartUV_EditorViewportClient::FSmartUV_EditorViewportClient(const TWeakPtr<SEditorViewport>& InEditorViewportWidget,
                                                             const TWeakPtr<FSmartUV_AssetEditorToolkit> InToolkit)
	: FEditorViewportClient(nullptr, nullptr, InEditorViewportWidget)
	  , SmartUV_EditorPtr(InToolkit)
{
	// Viewport Camera Settings
	SetOrthoZoom(700.f);
	SetViewLocation(FVector(50, -50, 0));
	SetViewModes(VMI_Lit, VMI_Lit);
	SetViewportType(LVT_OrthoXZ);

	// The interaction widget used by the SmartUV Editor **inherited 
	Widget->SetUsesEditorModeTools(ModeTools.Get());

	CurrentMode = ESmartUVEditorMode::ViewMode;

	// Set the preview scene
	PreviewScene = &OwnedPreviewScene;

	// Draw a grid in the viewport
	DrawHelper.bDrawGrid = true;

	// Preview scene setup
	{
		// Create a basic plane mesh that will be used in the preview scene to display the preview texture
		PreviewPlaneMesh = NewObject<UStaticMeshComponent>();
		UStaticMesh* PlaneMesh = CastChecked<UStaticMesh>(
			FAssetData(LoadObject<UStaticMesh>(nullptr, *UActorFactoryBasicShape::BasicPlane.ToString())).GetAsset());
		PreviewPlaneMesh->SetStaticMesh(PlaneMesh);

		// Offset the plane a bit to mimic a UV editor
		const FTransform PreviewPlaneTransform(FRotator(0, 0, 90), FVector(50, 0, 50), FVector::OneVector);
		PreviewScene->AddComponent(PreviewPlaneMesh, PreviewPlaneTransform);

		// Load the preview master material asset
		PreviewMaterial = LoadObject<UMaterial>(
			nullptr, TEXT("/Engine/EngineMaterials/EmissiveTexturedMaterial.EmissiveTexturedMaterial"), nullptr,
			LOAD_None, nullptr);

		// Create a dynamic preview material instance for the plane
		DynamicPreviewMaterial = UMaterialInstanceDynamic::Create(PreviewMaterial, GetTransientPackage());
	}

	// Activate the EditMode by default
	ActivateEditMode();

	// We want the viewport to be realtime so it reacts to changes of the preview material.
	// Didn't find anything in the base class similar to Redraw, Refresh or MarkRenderStateDirty so this will do
	AddRealtimeOverride(true, FText::FromString("Realtime"));

	Initialize();
}

void FSmartUV_EditorViewportClient::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize Called!"));

	const int NumIslands = SmartUV_EditorPtr.Pin()->SmartUV_Asset->UVIslands.Num();
	for (int i = 0; i < NumIslands; i++)
	{
		const FLinearColor IslandColor = FLinearColor::MakeRandomColor();
		
		FSmartUV_Island Island = SmartUV_EditorPtr.Pin()->SmartUV_Asset->UVIslands[i];
		SelectedIslands.Add(SelectedIsland(i, Island, IslandColor));
	}
}

FLinearColor FSmartUV_EditorViewportClient::GetBackgroundColor() const
{
	// Draw dark clear color so the viewport doesn't burn our eyes out
	return FLinearColor(0.01f, 0.01f, 0.01f);
}

void FSmartUV_EditorViewportClient::CheckHoveredHitProxy(HHitProxy* HoveredHitProxy)
{
	//@TODO: We should handle selections in SmartUV_EditMode but only this class
	// has a function to determine if we are hovering a hit proxy.
	// Can look at this later, not really important for now.

	FEditorViewportClient::CheckHoveredHitProxy(HoveredHitProxy);
}

void FSmartUV_EditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
}

void FSmartUV_EditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	// Draw all the UV Islands
	DrawUVIslands(Canvas);

	// Call to parent
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);
}

void FSmartUV_EditorViewportClient::DrawUVIslands(FCanvas& Canvas)
{
	//@TODO: We don't want to draw the UVIslands directly, we want to store them first in a struct that is initialized once that has information
	//@TODO: about the selection and other things.
	// Draw each island in the asset
	for (auto UVIsland : SelectedIslands)
	{
		const bool bIsHitTesting = Canvas.IsHitTesting();
		if (!bIsHitTesting)
		{
			Canvas.SetHitProxy(nullptr);
		}
		
		UVIslandRect Island = UVIslandRect(UVIsland.Island);
		TArray<FVector> Vertices = Island.GetIslandVertices(Island.ConvertToWSCoords());

		TArray<FVector2D> UVScreenspace;
		for (auto Vertex : Vertices)
		{
			UVScreenspace.Add(ProjectWorldLocationToScreen(this, Vertex));
		}
		

		if(bIsHitTesting)
		{
			TSharedPtr<FSelectedItem> TData = MakeShareable(new FSelectedItem());
			Canvas.SetHitProxy(new HIslandSelectableObjectHitProxy(TData));			
		}

		const FVector2d TileSize = (UVScreenspace[2] - UVScreenspace[0]);
		Canvas.DrawTile(UVScreenspace[0].X, UVScreenspace[0].Y, TileSize.X, TileSize.Y, 0,0,1,1,UVIsland.GetIslandColor(), GWhiteTexture);
		
		if (bIsHitTesting)
		{
			Canvas.SetHitProxy(nullptr);
		}
	}
		
	// 	// For each vertex
	// 	for (int VertexIndex = 0; VertexIndex < 4; VertexIndex++)
	// 	{
	// 		const int NewVertexIndex = (VertexIndex + 1) % 4;
	//
	// 		// Draw lines
	// 		const FLinearColor ShapeColor = FLinearColor::White;
	// 		FCanvasLineItem LineItem(UVScreenspace[VertexIndex], UVScreenspace[NewVertexIndex]);
	// 		LineItem.SetColor(ShapeColor);
	// 		Canvas.DrawItem(LineItem);
	//
	// 		// Draw Corners
	// 		const FVector2D VertBoxSize = FVector2D(5);
	// 		FVector2D VertexBoxPos = UVScreenspace[VertexIndex];
	// 		VertexBoxPos.X -= VertBoxSize.X / 2; // Offset position so the vert is in the middle
	// 		VertexBoxPos.Y -= VertBoxSize.X / 2;
	//
	// 		// Add corner hit proxy
	// 		const FVector2D CornerPoint = UVScreenspace[VertexIndex];
	// 		constexpr float CornerCollisionVertexSize = 8.0f;
	//
	// 		if (bIsHitTesting)
	// 		{
	// 			TSharedPtr<FSelectedItem> Data = MakeShareable(new FSelectedItem());
	// 			Data->VertexIndex = VertexIndex;
	// 			Canvas.SetHitProxy(new HIslandSelectableObjectHitProxy(Data));
	// 		}
	//
	// 		Canvas.DrawTile(CornerPoint.X - CornerCollisionVertexSize * 0.5f,
	// 		                CornerPoint.Y - CornerCollisionVertexSize * 0.5f, CornerCollisionVertexSize,
	// 		                CornerCollisionVertexSize, 0.f, 0.f, 1.f, 1.f, FLinearColor::White, GWhiteTexture);
	//
	// 		if (bIsHitTesting)
	// 		{
	// 			Canvas.SetHitProxy(nullptr);
	// 		}
	//
	// 		TSharedPtr<FSelectedItem> Data = MakeShareable(new FSelectedItem());
	// 		Canvas.SetHitProxy(new HIslandSelectableObjectHitProxy(Data));
	// 	}
	// }
}

FIntPoint FSmartUV_EditorViewportClient::ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient,
                                                                      FVector InWorldSpaceLocation)
{
	FSceneView* View = GetSceneView(InViewportClient);

	FVector2D OutScreenPos;
	FMatrix const ViewProjectionMatrix = View->ViewMatrices.GetViewProjectionMatrix();
	View->ProjectWorldToScreen(InWorldSpaceLocation, View->UnscaledViewRect, ViewProjectionMatrix, OutScreenPos);

	return FIntPoint(OutScreenPos.X, OutScreenPos.Y);
}

FSceneView* FSmartUV_EditorViewportClient::GetSceneView(class FEditorViewportClient* InViewportClient)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		InViewportClient->Viewport,
		InViewportClient->GetScene(),
		InViewportClient->EngineShowFlags));

	FSceneView* View = InViewportClient->CalcSceneView(&ViewFamily);

	return View;
}

void FSmartUV_EditorViewportClient::ActivateEditMode() const
{
	// Activate the SmartUV edit mode
	ModeTools->SetDefaultMode(FSmartUV_EditorMode::EM_SmartUV_EditorModeID);
	ModeTools->ActivateDefaultMode();

	FSmartUV_EditorMode* EditMode = ModeTools->GetActiveModeTyped<FSmartUV_EditorMode>(
		FSmartUV_EditorMode::EM_SmartUV_EditorModeID);
	check(EditMode);
	ModeTools->SetWidgetMode(UE::Widget::WM_Translate);
}

void FSmartUV_EditorViewportClient::UpdatePreviewMaterial(UTexture* InTexture) const
{
	// Set a new preview texture
	DynamicPreviewMaterial->SetTextureParameterValue(FName(TEXT("Texture")), InTexture);
	PreviewPlaneMesh->SetMaterial(0, DynamicPreviewMaterial);
}

void FSmartUV_EditorViewportClient::AddBoxButtonClicked() const
{
	FSmartUV_Island newIsland;
	newIsland.BottomLeft = FVector2D(-1, -1);
	newIsland.TopRight = FVector2D(0, 0);
	SmartUV_EditorPtr.Pin()->SmartUV_Asset->UVIslands.Add(newIsland);
	UE_LOG(LogTemp, Warning, TEXT("New island added!"));
}
