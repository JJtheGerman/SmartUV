#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "SmartUV_Asset.generated.h"

USTRUCT(BlueprintType)
struct FSmartUV_Island
{
	GENERATED_BODY()

	FVector2D BottomLeft;
	FVector2D TopRight;
};


UCLASS(BlueprintType, hidecategories = Object)
class SMARTUV_API USmartUV_Asset : public UObject
{
	GENERATED_BODY()

public:

	/** Holds the stored UV islands. */
	UPROPERTY()
	TArray<FSmartUV_Island> UVIslands;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General")
	TArray<class UMaterialInterface*> AssociatedMaterials;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General|Preview")
	class UTexture* PreviewTexture;
};