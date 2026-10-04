// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
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
	if (ActiveGrid.IsValid()) ActiveGrid->HiddenCursor();
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

