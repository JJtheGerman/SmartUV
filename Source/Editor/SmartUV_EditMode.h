#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"


class FSmartUV_EditorMode : public UEdMode
{
public:
	
	FSmartUV_EditorMode();

	const static FEditorModeID EM_SmartUV_EditorModeID;

	// FEdMode interface
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	// End of FEdMode interface
};