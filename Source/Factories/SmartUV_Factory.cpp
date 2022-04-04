#include "Factories/SmartUV_Factory.h"
#include "Asset/SmartUV_Asset.h"


USmartUV_FactoryNew::USmartUV_FactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = USmartUV_Asset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* USmartUV_FactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USmartUV_Asset>(InParent, InClass, InName, Flags);
}

bool USmartUV_FactoryNew::ShouldShowInNewMenu() const
{
	return true;
}