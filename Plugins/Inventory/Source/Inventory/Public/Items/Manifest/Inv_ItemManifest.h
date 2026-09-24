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
	
	
	template<typename T>
	requires std::derived_from<T, FInv_ItemFragment>	//传入的T 只能是FInv_ItemFragment的派生
	const T* GetFragmentOfTypeWitchTag(const FGameplayTag& Tag) const;
	
	template<typename T>
	requires std::derived_from<T, FInv_ItemFragment>	//传入的T 只能是FInv_ItemFragment的派生
	const T* GetFragmentOfType() const;
	
	template<typename T>
	requires std::derived_from<T, FInv_ItemFragment>	//传入的T 只能是FInv_ItemFragment的派生
	T* GetFragmentOfTypeMutable();
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory" , meta = (ExcludeBaseStruct))//排除父类结构体（不能选父类结构体）
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;	//数组
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory{EInv_ItemCategory::None};

	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;

};

template <typename T> 
requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfTypeWitchTag(const FGameplayTag& Tag) const
{
	for (const auto& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(Tag)) continue;
			return FragmentPtr;
		}
	}
	return nullptr;
}



template <typename T> 
requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfType() const
{
	for (const auto& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}



template <typename T> requires std::derived_from<T, FInv_ItemFragment>
T* FInv_ItemManifest::GetFragmentOfTypeMutable()
{
	for (auto& Fragment : Fragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}


