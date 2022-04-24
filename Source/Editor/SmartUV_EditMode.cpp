#include "SmartUV_EditMode.h"

#include "EditorModeManager.h"
#include "SmartUV_EditorViewportClient.h"
#include "Toolkits/ToolkitManager.h"


DEFINE_LOG_CATEGORY(LogSmartUVEdMode);

#define LOCTEXT_NAMESPACE "FSmartUV_EdMode"

const FEditorModeID FSmartUV_EditorMode::EM_SmartUV_EditorModeID(TEXT("EM_SmartUV_EditorModeID"));

FSmartUV_EditorMode::FSmartUV_EditorMode()
{
	Info = FEditorModeInfo(
		FSmartUV_EditorMode::EM_SmartUV_EditorModeID,
		LOCTEXT("SmartUVEditMode", "SmartUV Edit Mode"),
		FSlateIcon(),
		false);
}

bool FSmartUV_EditorMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if(auto Proxy = HitProxyCast<HIslandSelectableObjectHitProxy>(HitProxy))
	{
		//UE_LOG(LogSmartUVEdMode, Display, TEXT("VertexIndex: %d"), Proxy->Data->VertexIndex);
		UE_LOG(LogSmartUVEdMode, Warning, TEXT("HitProxy"));
	}
	
	return FEdMode::HandleClick(InViewportClient, HitProxy, Click);
}

bool FSmartUV_EditorMode::ShouldDrawWidget() const
{
	//@TODO: Pseudo Code
	// if(HasItemSelected()) return true;
	
	// If we want to display the transform gizmo
	return true;
}

bool FSmartUV_EditorMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	return FEdMode::InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
}

#undef LOCTEXT_NAMESPACE
