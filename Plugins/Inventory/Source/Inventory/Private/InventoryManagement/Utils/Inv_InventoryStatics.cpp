

#include "InventoryManagement/Utils/Inv_InventoryStatics.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Types/Inv_GridTypes.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"

UInv_InventoryComponent* UInv_InventoryStatics::GetInventoryComponent(const APlayerController* OwningPlayerController)
{
	if (!IsValid(OwningPlayerController)) return nullptr;
	
	UInv_InventoryComponent* InventoryComponent = OwningPlayerController->FindComponentByClass<UInv_InventoryComponent>();
	
	return IsValid(InventoryComponent) ? InventoryComponent : nullptr;
}

EInv_ItemCategory UInv_InventoryStatics::GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent)) return EInv_ItemCategory::None;
	return ItemComponent->GetItemManifest().GetItemCategory();
}

void UInv_InventoryStatics::ItemHoevred(APlayerController* PC, UInv_InventoryItem* Item)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PC);
	if (!IsValid(InventoryComponent)) return;
	
	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;
	
	if (InventoryBase->HasHoverItem()) return;
	
	InventoryBase->OnItemHovered(Item);		//实际上就是调用Inv_SpatialInventory的 OnItemHovered
}

void UInv_InventoryStatics::ItemUnhovered(APlayerController* PC)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PC);
	if (!IsValid(InventoryComponent)) return;
	
	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;
	
	
	InventoryBase->OnItemUnhovered();	//实际上就是调用Inv_SpatialInventory的 OnItemUnhovered
}

UInv_HoverItem* UInv_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PC);
	if (!IsValid(InventoryComponent)) return nullptr;
	
	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;
	
	return InventoryBase->GetHoverItem();
	
}

UInv_InventoryBase* UInv_InventoryStatics::GetInvntoryWidget(APlayerController* PC)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PC);
	if (!IsValid(InventoryComponent)) return nullptr;
	
	return InventoryComponent->GetInventoryMenu();
}

