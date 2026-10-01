#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"


UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter ,UInv_InventoryItem::StaticClass());	//创建空壳
	
	Item->SetItemManifest(*this);	//灌数据（目前只有EItemCategory 和 Fragment）
	
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
