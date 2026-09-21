#include "InventoryManagement/FastArray/Inv_FastArray.h"

#include "ToolMenusEditor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Tests/ToolMenusTestUtilities.h"


TArray<UInv_InventoryItem*> FInv_InventoryFastArray::GetAllItems() const
{
	TArray<UInv_InventoryItem*> Results;
	
	Results.Reserve(Entries.Num());
	
	for (const auto& Entry : Entries)
	{
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}
	
	return Results;
}

void FInv_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	
	for (int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);	//只在客户端回调
	}
}

void FInv_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	
	for (int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_ItemComponent* ItemComponent)	//玩家捡起地上的东西
{
	check(OwnerComponent);
	AActor* OwningActor = Cast<AActor>(OwnerComponent->GetOwner());
	check(OwningActor->HasAuthority());
	
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return nullptr;
	
	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();	// 在数组末尾加一个默认构造的元素,返回引用
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);		//归OwningActor管
	
	IC->AddRepSubObj(NewEntry.Item);
	
	MarkItemDirty(NewEntry);
	
	return NewEntry.Item;
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_InventoryItem* Item)		//读档、交易、从一个容器挪到另一个
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();	// 在数组末尾加一个默认构造的元素,返回引用
	NewEntry.Item = Item;
	
	MarkItemDirty(NewEntry);
	return Item;
}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt ; ++EntryIt)
	{
		FInv_InventoryEntry& Entry = *EntryIt;
		if (Entry.Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}