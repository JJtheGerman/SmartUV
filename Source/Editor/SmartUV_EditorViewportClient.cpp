#include "SmartUV_EditorViewportClient.h"
#include "AssetEditorModeManager.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Asset/SmartUV_Asset.h"
#include "CanvasItem.h"

IMPLEMENT_HIT_PROXY(HIslandSelectableObjectHitProxy, HHitProxy)

FSmartUV_EditorViewportClient::FSmartUV_EditorViewportClient(const TWeakPtr<SEditorViewport>& InEditorViewportWidget, const TSharedPtr<FSmartUV_AssetEditorToolkit> InToolkit)
	: FEditorViewportClient(nullptr, nullptr, InEditorViewportWidget)
	,SmartUV_EditorPtr(InToolkit)
{
	SetViewModes(VMI_Lit, VMI_Lit);
	SetViewportType(LVT_OrthoXY);

	CurrentMode = ESmartUVEditorMode::ViewMode;

	// Set the preview scene, in this case its completely empty
	PreviewScene = &OwnedPreviewScene;

	// Draw a grid in the viewport
	DrawHelper.bDrawGrid = true;

	// Preview scene setup
	{
		// Initial camera position in the viewport (this appears broken? It does nothing...)
		SetInitialViewTransform(LVT_Perspective, FVector(0, 0, 0), FVector::UpVector.Rotation(), 0.f);
		SetOrthoZoom(700.f);

		// Create a basic plane mesh that will be used in the preview scene to display the preview texture
		PreviewPlaneMesh = NewObject<UStaticMeshComponent>();
		UStaticMesh* PlaneMesh = CastChecked<UStaticMesh>(FAssetData(LoadObject<UStaticMesh>(nullptr, *UActorFactoryBasicShape::BasicPlane.ToString())).GetAsset());
		PreviewPlaneMesh->SetStaticMesh(PlaneMesh);

		// Offset the plane a bit to mimic a UV editor
		PreviewScene->AddComponent(PreviewPlaneMesh, FTransform(FVector(50, -50, 0)));

		// Load the preview master material asset
		PreviewMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Engine/EngineMaterials/EmissiveTexturedMaterial.EmissiveTexturedMaterial"), nullptr, LOAD_None, nullptr);

		// Create a dynamic preview material instance for the plane
		DynamicPreviewMaterial = UMaterialInstanceDynamic::Create(PreviewMaterial, GetTransientPackage());
	}

	// We want the viewport to be realtime so it reacts to changes of the preview material.
	// Didn't find anything in the base class similar to Redraw, Refresh or MarkRenderStateDirty so this will do
	AddRealtimeOverride(true, FText::FromString("Realtime"));
}

FLinearColor FSmartUV_EditorViewportClient::GetBackgroundColor() const
{
	// Draw dark clear color so the viewport doesn't burn our eyes out
	return FLinearColor(0.01f,0.01f,0.01f);
}

void FSmartUV_EditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
}

void FSmartUV_EditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	const bool bIsHitTesting = Canvas.IsHitTesting();
	if (!bIsHitTesting)
	{
		Canvas.SetHitProxy(nullptr);
	}

	// Draw each island in the asset
	TArray<FSmartUV_Island> UVIslands = SmartUV_EditorPtr->SmartUV_Asset->UVIslands;
	for (FSmartUV_Island UVIsland : UVIslands)
	{
		UVIslandRect Island = UVIslandRect(UVIsland);
		TArray<FVector> Vertices = Island.GetIslandVertices(Island.ConvertToWSCoords());

		TArray<FVector2D> UVScreenspace;
		for (auto Vertex : Vertices)
		{
			UVScreenspace.Add(ProjectWorldLocationToScreen(this, Vertex));
		}

		for (int VertexIndex = 0; VertexIndex < 4; VertexIndex++)
		{
			const int NewVertexIndex = (VertexIndex + 1) % 4;

			// Draw lines
			FCanvasLineItem LineItem(UVScreenspace[VertexIndex], UVScreenspace[NewVertexIndex]);
			LineItem.SetColor(FLinearColor::White);
			Canvas.DrawItem(LineItem);

			// Draw Corners
			const FVector2D VertBoxSize = FVector2D(5);
			FVector2D VertexBoxPos = UVScreenspace[VertexIndex];
			VertexBoxPos.X -= VertBoxSize.X / 2; // Offset position so the vert is in the middle
			VertexBoxPos.Y -= VertBoxSize.X / 2;

			FCanvasBoxItem BoxItem(VertexBoxPos, VertBoxSize);
			BoxItem.SetColor(FLinearColor(1,1,1,0.99f));
			BoxItem.LineThickness = 5.f;
			Canvas.DrawItem(BoxItem);

			{
				// Add corner hit proxy
				const FVector2D CornerPoint = UVScreenspace[VertexIndex];
				const float CornerCollisionVertexSize = 8.0f;

				if (bIsHitTesting)
				{					
					TSharedPtr<FSelectedItem> Data = MakeShareable(new FSelectedItem());
					Data->VertexIndex = VertexIndex;
					Canvas.SetHitProxy(new HIslandSelectableObjectHitProxy(Data));
				}

				Canvas.DrawTile(CornerPoint.X - CornerCollisionVertexSize * 0.5f, CornerPoint.Y - CornerCollisionVertexSize * 0.5f, CornerCollisionVertexSize, CornerCollisionVertexSize, 0.f, 0.f, 1.f, 1.f, FLinearColor::White, GWhiteTexture);

				if (bIsHitTesting)
				{
					Canvas.SetHitProxy(nullptr);
				}

				TSharedPtr<FSelectedItem> Data = MakeShareable(new FSelectedItem());
				Canvas.SetHitProxy(new HIslandSelectableObjectHitProxy(Data));
			}
		}

	}

	// Call to parent
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);
}

void FSmartUV_EditorViewportClient::DrawSelectionRectangles(FViewport* InViewport, FCanvas* Canvas)
{
	//	Canvas->DrawTile(X, Y, W, H, 0, 0, 1, 1, Rect.Color, GWhiteTexture, bAlphaBlend);
}

FIntPoint FSmartUV_EditorViewportClient::ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient, FVector InWorldSpaceLocation)
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

void FSmartUV_EditorViewportClient::UpdatePreviewMaterial(UTexture* InTexture)
{
	// Set a new preview texture
	DynamicPreviewMaterial->SetTextureParameterValue(FName(TEXT("Texture")), InTexture);
	PreviewPlaneMesh->SetMaterial(0, DynamicPreviewMaterial);
}

void FSmartUV_EditorViewportClient::AddBoxButtonClicked()
{
	FSmartUV_Island newIsland = FSmartUV_Island();
	newIsland.BottomLeft = FVector2D(-1, -1);
	newIsland.TopRight = FVector2D(0, 0);
	SmartUV_EditorPtr->SmartUV_Asset->UVIslands.Add(newIsland);
	UE_LOG(LogTemp, Warning, TEXT("New island added!"));
}