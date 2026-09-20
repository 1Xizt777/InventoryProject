#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

class UInv_InventoryComponent;
class UInv_InventoryItem;
class UInv_ItemComponent;

//	记事本上的一行
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInv_InventoryEntry() {}		//默认构造函数。UE 反序列化时要能"凭空造一个

private:
	friend struct FInv_InventoryFastArray;		//只有整页和背包组件能碰Item
	friend UInv_InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;		
};



//	一整页（所有行）
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}		//无参的 → UE 复制/反序列化时需要
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}	//创建时用，顺手记住"我属于谁

	TArray<UInv_InventoryItem*> GetAllItems() const;

	
	// FFastArraySerializer contract	//客户端回调
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	// End of FFastArraySerializer contract

	
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{									//模板参数：<一行的类型, 整页的类型>
		return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* Item);
	void RemoveEntry(UInv_InventoryItem* Item);

private:
	friend UInv_InventoryComponent;

	// Replicated list of items
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;		//真正的数据

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;	//指回拥有这个背包的组件
};



//告诉引擎"这个 struct 自己会做 delta 序列化
template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};