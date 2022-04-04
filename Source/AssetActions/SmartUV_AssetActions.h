#pragma once

#include "AssetTypeActions_Base.h"
#include "Templates/SharedPointer.h"

class FSmartUV_AssetActions : public FAssetTypeActions_Base
{

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InStyle The style set to use for asset editor toolkits.
	 */
	FSmartUV_AssetActions();

public:

	// Overrides
	virtual bool CanFilter() override;
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
};