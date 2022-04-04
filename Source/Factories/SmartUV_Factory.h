#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "SmartUV_Factory.generated.h"


/**
 * Implements a factory for USmartMaterial objects.
 */
UCLASS(hidecategories = Object)
class SMARTUV_API USmartUV_FactoryNew : public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	//~ UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};