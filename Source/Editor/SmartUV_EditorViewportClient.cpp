#include "SmartUV_EditorViewportClient.h"
#include "AssetEditorModeManager.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Asset/SmartUV_Asset.h"
#include "CanvasItem.h"


FSmartUV_EditorViewportClient::FSmartUV_EditorViewportClient(const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(nullptr, nullptr, InEditorViewportWidget)
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
	//TArray<FSmartUV_Island> UVIslands = SmartUV_EditorPtr->SmartUV_Asset->UVIslands;

	//for (FSmartUV_Island UVIsland : UVIslands)
	//{
	//	for (int VertexIndex = 0; VertexIndex < 4; ++VertexIndex)
	//	{
	//		const int NextVertexIndex = (VertexIndex + 1) % 4;



	//		FCanvasLineItem LineItem(BoundsVertices[VertexIndex], BoundsVertices[NextVertexIndex]);
	//		LineItem.SetColor(FLinearColor::White);
	//		Canvas.DrawItem(LineItem);
	//	}
	//}

	// World space
	const FVector2D BottomLeft(10,10);
	const FVector2D TopRight(90,90);

	// Vertices of a box in World space
	TArray<FVector> BoxWorldSpace;
	{
		BoxWorldSpace.Add(FVector(BottomLeft.X, BottomLeft.Y, 0.f)); // BottomLeft
		BoxWorldSpace.Add(FVector(TopRight.X, BottomLeft.Y, 0.f));   // BottomRight
		BoxWorldSpace.Add(FVector(TopRight.X, TopRight.Y, 0.f));     // TopRight
		BoxWorldSpace.Add(FVector(BottomLeft.X, TopRight.Y, 0.f));   // TopLeft
	}

	TArray<FVector2D> BoxScreenSpace;
	{
		BoxScreenSpace.Add((ProjectWorldLocationToScreen(this, BoxWorldSpace[0], false)));
		BoxScreenSpace.Add((ProjectWorldLocationToScreen(this, BoxWorldSpace[1], false)));
		BoxScreenSpace.Add((ProjectWorldLocationToScreen(this, BoxWorldSpace[2], false)));
		BoxScreenSpace.Add((ProjectWorldLocationToScreen(this, BoxWorldSpace[3], false)));
	}

	for (int VertexIndex = 0; VertexIndex < 4; VertexIndex++)
	{
		const int NewVertexIndex = (VertexIndex + 1) % 4;

		// Draw lines
		//FCanvasLineItem LineItem(FVector2D(200, 500), FVector2D(900, 500));
		FCanvasLineItem LineItem(BoxScreenSpace[VertexIndex], BoxScreenSpace[NewVertexIndex]);
		LineItem.SetColor(FLinearColor::White);
		Canvas.DrawItem(LineItem);
	}

	// Call to parent
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);
}

void FSmartUV_EditorViewportClient::DrawSelectionRectangles(FViewport* InViewport, FCanvas* Canvas)
{
	//	Canvas->DrawTile(X, Y, W, H, 0, 0, 1, 1, Rect.Color, GWhiteTexture, bAlphaBlend);
}

FIntPoint FSmartUV_EditorViewportClient::ProjectWorldLocationToScreen(class FEditorViewportClient* InViewportClient, FVector InWorldSpaceLocation, bool InClampValues)
{
	FSceneView* View = GetSceneView(InViewportClient);

	FVector2D OutScreenPos;
	FMatrix const ViewProjectionMatrix = View->ViewMatrices.GetViewProjectionMatrix();
	View->ProjectWorldToScreen(InWorldSpaceLocation, View->UnscaledViewRect, ViewProjectionMatrix, OutScreenPos);

	//Clamp Values because ProjectWorldToScreen can give you negative values...
	if (InClampValues)
	{
		FIntPoint ViewportResolution = InViewportClient->Viewport->GetSizeXY();
		OutScreenPos.X = FMath::Clamp((int32)OutScreenPos.X, 0, ViewportResolution.X);
		OutScreenPos.Y = FMath::Clamp((int32)OutScreenPos.Y, 0, ViewportResolution.Y);
	}

	return FIntPoint(OutScreenPos.X, OutScreenPos.Y);
}

FSceneView* FSmartUV_EditorViewportClient::GetSceneView(class FEditorViewportClient* InViewportClient)
{
	FViewportCursorLocation MousePosition = InViewportClient->GetCursorWorldLocationFromMousePos();
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
	UE_LOG(LogTemp, Warning, TEXT("Test button!"));
}
