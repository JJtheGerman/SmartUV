#include "SmartUV_EditorViewportClient.h"
#include "AssetEditorModeManager.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "ActorFactories/ActorFactoryBasicShape.h"
#include "Materials/MaterialInstanceDynamic.h"


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
}

FLinearColor FSmartUV_EditorViewportClient::GetBackgroundColor() const
{
	return FLinearColor::Black;
}

void FSmartUV_EditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
}

void FSmartUV_EditorViewportClient::DrawSelectionRectangles(FViewport* InViewport, FCanvas* Canvas)
{
	//	Canvas->DrawTile(X, Y, W, H, 0, 0, 1, 1, Rect.Color, GWhiteTexture, bAlphaBlend);
}

void FSmartUV_EditorViewportClient::UpdatePreviewMaterial(UTexture* InTexture)
{
	// Set a new preview texture
	DynamicPreviewMaterial->SetTextureParameterValue(FName(TEXT("Texture")), InTexture);
	PreviewPlaneMesh->SetMaterial(0, DynamicPreviewMaterial);
}