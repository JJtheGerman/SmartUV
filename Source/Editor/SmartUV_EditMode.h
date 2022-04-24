#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSmartUVEdMode, Display, All);

class FSmartUV_EditorMode : public FEdMode
{

public:

	// Mode ID
	const static FEditorModeID EM_SmartUV_EditorModeID;
	
public:

	// Constructor
	FSmartUV_EditorMode();

public:

	// FEdMode Interface
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool ShouldDrawWidget() const override; // If we want to draw the transform gizmo
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	// FEdMode Interface
	
};