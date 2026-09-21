#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"




UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter ,UInv_InventoryItem::StaticClass());	//创建空壳
	
	Item->SetItemManifest(*this);	//灌数据（目前只有EItemCategory）
	
	return Item;
}