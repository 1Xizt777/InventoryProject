#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"


UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter ,UInv_InventoryItem::StaticClass());	//创建空壳
	
	Item->SetItemManifest(*this);	//灌数据     ！！！   this(临时副本) → 深拷贝 → Item 里多了一份
	
	for (auto& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();	//调用每个FInv_ItemFragment以及其子类的Manifest，无论是否是空实现
	}
	
	
	ClearFragments();	//清理的是FInv_InventoryFastArray::AddEntry里的临时无名副本（清的是 this(临时副本)）
	
	return Item;
	
}


void FInv_ItemManifest::SpawnPickUpActor(const UObject* WorldContextObject, const FVector& SpawnLocation,const FRotator& SpawnRotation)
{
	if (!IsValid(PickUpActorClass) || !IsValid(WorldContextObject)) return;
	
	AActor* SpawnActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickUpActorClass , SpawnLocation , SpawnRotation);
	if (!IsValid(SpawnActor)) return;

	UInv_ItemComponent* ItemComponent = SpawnActor->FindComponentByClass<UInv_ItemComponent>();
	if (!IsValid(ItemComponent)) return;
	
	ItemComponent->InitialMainfest(*this);
}



void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	const auto& InventoryItemFragments = GetAllFragmentsOfType<FInv_InventoryItemFragment>();
	
	for (const auto& Fragment : InventoryItemFragments)	//遍历所有InventoryItemFragment以及其子类
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)	//Widget 就是描述框控件树里当前被遍历到的那个节点
		{
			Fragment->Assimilate(Widget);
		});
	}
	
}


void FInv_ItemManifest::ClearFragments()
{
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}
	Fragments.Empty();
}