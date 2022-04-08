#include "SmartUV_EditorCommands.h"

#define LOCTEXT_NAMESPACE "SmartUVEditor"

void FSmartUVEditorCommands::RegisterCommands()
{
	// Show toggles
	UI_COMMAND(AddBox, "Add Box", "Adds a new UV island", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE