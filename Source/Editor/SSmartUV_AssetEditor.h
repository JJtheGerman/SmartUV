#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

class USmartUV_Asset;

class SSmartUV_AssetEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SSmartUV_AssetEditor) { }
	SLATE_END_ARGS()

public:

	virtual ~SSmartUV_AssetEditor();
	void Construct(const FArguments& InArgs, USmartUV_Asset* InSmartMaterialAsset, const TSharedRef<ISlateStyle>& InStyle);

private:

	// Pointer to the SmartUV asset that is being edited
	USmartUV_Asset* SmartMaterialAsset;
};