// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Equippables->OnClicked.AddDynamic(this,&UInv_SpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this,&UInv_SpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this,&UInv_SpatialInventory::ShowCraftables);
	
	Grid_Consumables->SetOwningCanvasPanel(CanvasPanel);
	Grid_Craftables->SetOwningCanvasPanel(CanvasPanel);
	Grid_Equippables->SetOwningCanvasPanel(CanvasPanel);
	
	ShowEquippables();
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->OnEquippedGridSlotClicked.AddDynamic(this , &ThisClass::OnEquippedGridSlotClicked);
		}
	});
	
}


void UInv_SpatialInventory::OnEquippedGridSlotClicked(UInv_EquippedGridSlot* EuippGridSlot,const FGameplayTag& EquipmentTypeTag)
{
	if (!IsValid(EuippGridSlot)) return;
	
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return;
	
	//1.通过HoverItem判断是否能装备
	if (!CanEquipHoverItem(EuippGridSlot , EquipmentTypeTag)) return;
	
	
	//计算TileSize
	const float TileSize = UInv_InventoryStatics::GetInvntoryWidget(GetOwningPlayer())->GetTileSize();
	
	//创建EquippedSlottedItem
	UInv_EquippedSlottedItem* EquippedSlottedItem =  EuippGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTypeTag,
		TileSize);
	
	//绑定广播	(主要处理Unequip等相关逻辑)
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this , &ThisClass::EquippedSlottedItemClicked);
	
	//清理HoverItem
	Grid_Equippables->ClearHoverItem();
	
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	// check(IsValid(InventoryComponent));
	
	//告诉服务器装备了
	InventoryComponent->Server_EquipSlotItemClicked(HoverItem->GetInventoryItem() , nullptr);
	
	
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)	//不在专用服务器
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());	//（这次广播是给本地用的）
	}
		
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot,const FGameplayTag& EquipmentTypeTag)
{
	if (!IsValid(EquippedGridSlot)) return false;
	
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;
	
	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();
	
	
	return	IsValid(HeldItem) 
			&& HasHoverItem() 
			&& !HoverItem->IsStackable()
			&& HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable
			&& HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	//去掉ItemDescription
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());

	if (!IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;

	//获取要装备的item
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
		
	//获取要卸下的物品
	UInv_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();
	
	//找到要装备的那个格子的GridSlot
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	
	//清理EquippedGridSlot的Item和EquippedSlottedItem
	ClearSlotOfItem(EquippedGridSlot);
	
	//创建取下来物品的HoverItem
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	
	//解绑广播，并且删掉SlottedItem
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	
	//创建新的SlottedItem
	MakeEquippedSlottedItem(EquippedSlottedItem , EquippedGridSlot , ItemToEquip);
	
	
	
}
UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem](const UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == EquippedItem;
	});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UInv_SpatialInventory::RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;
	
	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this , &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this , &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem,UInv_EquippedGridSlot* EquippedGridSlot,
	UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;
	
	UInv_EquippedSlottedItem* NewEquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip ,
		EquippedSlottedItem->GetEquipmentTypeTag() ,
		UInv_InventoryStatics::GetInvntoryWidget(GetOwningPlayer())->GetTileSize()
		);
	
	if (IsValid(NewEquippedSlottedItem))
	{
		NewEquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this , &ThisClass::EquippedSlottedItemClicked);
	}
	
	EquippedGridSlot->SetEquippedSlottedItem(NewEquippedSlottedItem);
}

void UInv_SpatialInventory::BroadcastSlotClickedChanged(UInv_InventoryItem* ItemToEquip,UInv_InventoryItem* ItemToUnEquip)
{
	if (!IsValid(GetOwningPlayer())) return;
	
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	if (!IsValid(InventoryComponent)) return;
	
	InventoryComponent->Server_EquipSlotItemClicked(ItemToEquip ,ItemToUnEquip);
	
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnEquip);
	}
}


FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(ItemDescription)) return;
	
	SetItemDescriptionSizeAndPosition(ItemDescription , CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* ItemDescriptionWidget,UCanvasPanel* Canvas)
{
	if (!IsValid(CanvasPanel)) return;
	
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemDescriptionWidget);
	if (!IsValid(ItemDescriptionCPS)) return;
	
	const FVector2D ItemDescriptionSize = ItemDescriptionWidget->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);
	
	FVector2D ClampedWidgetPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer())
		);
	
	ItemDescriptionCPS->SetPosition(ClampedWidgetPosition);
}




FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent))
	{
		case EInv_ItemCategory::Equippable:
			return Grid_Equippables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Consumable:
			return Grid_Consumables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Craftable:
			return Grid_Craftables->HasRoomForItem(ItemComponent);
		default:
		UE_LOG(LogInventory, Error , TEXT("Item Component doesn't have a valid ItemCategory!"))
		return FInv_SlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	
	UInv_ItemDescription* DescriptionWidget = GetItemDescriptionWidget();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	
	
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimerHandle);
	
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this , Manifest, DescriptionWidget]()
	{
		Manifest.AssimilateInventoryFragments(DescriptionWidget);
		GetItemDescriptionWidget()->SetVisibility(ESlateVisibility::HitTestInvisible);
	});
	
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(
		DescriptionTimerHandle,
		DescriptionTimerDelegate , 
		DescriptionTimerDelay,
		false);
	
}

void UInv_SpatialInventory::OnItemUnhovered()
{
	GetItemDescriptionWidget()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimerHandle);
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;
	if (Grid_Equippables->HasHoverItem()) return true;
	return false;
}

UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* InventoryGrid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->OnHide();
		ActiveGrid->HiddenCursor();
	}
	ActiveGrid = InventoryGrid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	
	Switcher->SetActiveWidget(InventoryGrid);
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescriptionWidget()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer() , ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

