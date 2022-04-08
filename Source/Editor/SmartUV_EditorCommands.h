#pragma once

#include "SlateBasics.h"
#include "EditorStyleSet.h"

class FSmartUVEditorCommands : public TCommands<FSmartUVEditorCommands>
{
public:
	FSmartUVEditorCommands()
		: TCommands<FSmartUVEditorCommands>
		(
			TEXT("SmartUV_AssetEditor"),
			NSLOCTEXT("Contexts", "SmartUV_AssetEditor", "SmartUV AssetEditor"),
			NAME_None,
			FEditorStyle::GetStyleSetName()
			) {}

	// TCommand<> interface
	virtual void RegisterCommands() override;
	// End of TCommand<> interface
	
public:

	TSharedPtr<FUICommandInfo> AddBox;
};