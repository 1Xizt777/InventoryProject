#include "Items/Inv_InventoryItem.h"
#include "Items/Manifest/Inv_ItemManifest.h"





UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter ,UInv_InventoryItem::StaticClass());
	
	Item->SetItemManifest(*this);
	
	return Item;
}