#pragma once

#include "Inv_GridTypes.generated.h"

class UInv_InventoryItem;

UENUM(BlueprintType)
enum class EInv_ItemCategory : uint8
{
	None,
	Equippable,
	Consumable,
	Craftable
};



USTRUCT()
struct FInv_SlotAvailability
{
	GENERATED_BODY()

	FInv_SlotAvailability() {}
	FInv_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}
	
	//「格子容量信息」（描述"第几个格子，能装多少，那里本来有没有东西"）
	
	int32 Index{INDEX_NONE};		//这个格子的下标
	int32 AmountToFill{0};			//这个格子能装下多少个
	bool bItemAtIndex{false};		//这个格子上本来有没有同一物品
	
};



USTRUCT()
struct FInv_SlotAvailabilityResult
{
	GENERATED_BODY()

	FInv_SlotAvailabilityResult() {}

	TWeakObjectPtr<UInv_InventoryItem> Item;				//这次要放的那件物品
	int32 TotalRoomToFill{0};								//所有格子加起来一共能装多少个
	int32 Remainder{0};										//装完之后还剩多少没装下
	bool bStackable{false};									//这件物品能不能堆叠
	TArray<FInv_SlotAvailability> SlotAvailabilities;		//每个能放的格子的明细
};