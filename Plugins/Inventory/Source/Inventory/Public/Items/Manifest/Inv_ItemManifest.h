#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

struct FInv_ItemFragment;
enum class EInv_ItemCategory : uint8;

USTRUCT()
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()

public:
	
	UInv_InventoryItem* Manifest(UObject* NewOuter);
	
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory" , meta = (ExcludeBaseStruct))//排除父类结构体（不能选父类结构体）
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;	//数组
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory{EInv_ItemCategory::None};

	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;

};


