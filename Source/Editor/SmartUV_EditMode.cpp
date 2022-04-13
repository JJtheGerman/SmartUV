#include "SmartUV_EditMode.h"


#define LOCTEXT_NAMESPACE "FSmartUV_EdMode"

const FEditorModeID FSmartUV_EditorMode::EM_SmartUV_EditorModeID = TEXT("EM_SmartUV_EditorModeID");

FSmartUV_EditorMode::FSmartUV_EditorMode()
{
	Info = FEditorModeInfo(
		FSmartUV_EditorMode::EM_SmartUV_EditorModeID,
		LOCTEXT("SmartUVEditMode", "SmartUV Edit Mode"),
		FSlateIcon(),
		false);
}

// FEdMode: Called every frame as long as the Mode is active
void FSmartUV_EditorMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("SmartUV Mode called!"));
}


#undef LOCTEXT_NAMESPACE