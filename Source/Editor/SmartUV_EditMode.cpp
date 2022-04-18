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

#undef LOCTEXT_NAMESPACE