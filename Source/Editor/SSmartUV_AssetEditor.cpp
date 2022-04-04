#include "SSmartUV_AssetEditor.h"

#include "Fonts/SlateFontInfo.h"
#include "Internationalization/Text.h"
#include "UObject/Class.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

#include "Asset/SmartUV_Asset.h"

#define LOCTEXT_NAMESPACE "SSmartMaterialAssetEditor"

SSmartUV_AssetEditor::~SSmartUV_AssetEditor()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

void SSmartUV_AssetEditor::Construct(const FArguments& InArgs, USmartUV_Asset* InSmartMaterialAsset, const TSharedRef<ISlateStyle>& InStyle)
{
	SmartMaterialAsset = InSmartMaterialAsset;
}

#undef LOCTEXT_NAMESPACE