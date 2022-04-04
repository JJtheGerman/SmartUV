#include "SmartUV_EditorViewportClient.h"
#include "AssetEditorModeManager.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "SmartUV_AssetEditorToolkit.h"
#include "ActorFactories/ActorFactoryBasicShape.h"


FSmartUV_EditorViewportClient::FSmartUV_EditorViewportClient(const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(nullptr, nullptr, InEditorViewportWidget)
	, CheckerboardTexture(nullptr)
{
	ZoomPos = FVector2D::ZeroVector;
	ZoomAmount = 1.0f;

	SetViewModes(VMI_Lit, VMI_Lit);
	SetViewportType(LVT_OrthoXY);

	bDeferZoomToSprite = true;
	bDeferZoomToSpriteIsInstant = true;

	CurrentMode = ESmartUVEditorMode::ViewMode;

	// Set the preview scene, in this case its completely empty
	PreviewScene = &OwnedPreviewScene;

	// Draw a grid in the viewport
	DrawHelper.bDrawGrid = true;

	// Initial camera position in the viewport (which doesn't seem to work correctly)
	SetInitialViewTransform(LVT_Perspective, FVector(0,0,100.f), FVector::UpVector.Rotation(), 0.0f);

	// Create a basic plane mesh that will be used in the preview scene to display the preview texture
	{
		UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>();
		UStaticMesh* PlaneMesh = CastChecked<UStaticMesh>(FAssetData(LoadObject<UStaticMesh>(nullptr, *UActorFactoryBasicShape::BasicPlane.ToString())).GetAsset());
		Component->SetStaticMesh(PlaneMesh);

		// Offset the plane a bit to mimic a UV editor
		PreviewScene->AddComponent(Component, FTransform(FVector(50, -50, 0)));
	}
}

FLinearColor FSmartUV_EditorViewportClient::GetBackgroundColor() const
{
	return FLinearColor::Black;
}

void FSmartUV_EditorViewportClient::Tick(float DeltaSeconds)
{
	// Zoom in on the sprite
	//@TODO: Fix this properly so it doesn't need to be deferred, or wait for the viewport to initialize
	FIntPoint Size = Viewport->GetSizeXY();
	if (bDeferZoomToSprite && (Size.X > 0) && (Size.Y > 0))
	{
		FBox BoundsToFocus = GetDesiredFocusBounds();
		if (ViewportType != LVT_Perspective)
		{
			TGuardValue<ELevelViewportType> SaveViewportType(ViewportType, LVT_Perspective);
			FocusViewportOnBox(BoundsToFocus, bDeferZoomToSpriteIsInstant);
		}

		FocusViewportOnBox(BoundsToFocus, bDeferZoomToSpriteIsInstant);
		bDeferZoomToSprite = false;
	}

	FEditorViewportClient::Tick(DeltaSeconds);
}

void FSmartUV_EditorViewportClient::DrawSelectionRectangles(FViewport* InViewport, FCanvas* Canvas)
{
	//	Canvas->DrawTile(X, Y, W, H, 0, 0, 1, 1, Rect.Color, GWhiteTexture, bAlphaBlend);
}

void FSmartUV_EditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEditorViewportClient::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(CheckerboardTexture);
}

// Called to request a focus on the current selection
void FSmartUV_EditorViewportClient::RequestFocusOnSelection(bool bInstant)
{
	bDeferZoomToSprite = true;
	bDeferZoomToSpriteIsInstant = bInstant;
}

void FSmartUV_EditorViewportClient::ModifyCheckerboardTextureColors()
{
	const FColor ColorOne = FColor(128, 128, 128);
	const FColor ColorTwo = FColor(64, 64, 64);
	const int32 CheckerSize = 32;

	DestroyCheckerboardTexture();
	SetupCheckerboardTexture(ColorOne, ColorTwo, CheckerSize);
}

void FSmartUV_EditorViewportClient::SetupCheckerboardTexture(const FColor& ColorOne, const FColor& ColorTwo, int32 CheckerSize)
{
	if (CheckerboardTexture == nullptr)
	{
		CheckerboardTexture = FImageUtils::CreateCheckerboardTexture(ColorOne, ColorTwo, CheckerSize);
	}
}

void FSmartUV_EditorViewportClient::DestroyCheckerboardTexture()
{
	if (CheckerboardTexture)
	{
		if (CheckerboardTexture->Resource)
		{
			CheckerboardTexture->ReleaseResource();
		}

		CheckerboardTexture->MarkPendingKill();
		CheckerboardTexture = nullptr;
	}
}
