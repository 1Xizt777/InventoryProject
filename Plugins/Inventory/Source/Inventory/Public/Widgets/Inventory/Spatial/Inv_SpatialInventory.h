
#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "GameplayTagContainer.h"
#include "Inv_SpatialInventory.generated.h"

 
class UInv_EquippedSlottedItem;
class UInv_EquippedGridSlot;
class UInv_ItemDescription;
class UCanvasPanel;
class UInv_InventoryGrid;
class UWidgetSwitcher;
class UButton;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const override;
	
	
	virtual void OnItemHovered(UInv_InventoryItem* Item) override;
	virtual void OnItemUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UInv_HoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const override;
private:
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_EquippedGridSlot>> EquippedGridSlots;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equippables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Craftables;
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Equippables;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consumables;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Craftables;
	
	
	UFUNCTION()
	void ShowEquippables();
	
	UFUNCTION()
	void ShowConsumables();
	
	UFUNCTION()
	void ShowCraftables();
	
	void DisableButton(UButton* Button);

	void SetActiveGrid(UInv_InventoryGrid* InventoryGrid , UButton* Button);
	
	
	TWeakObjectPtr<UInv_InventoryGrid> ActiveGrid;
	
	
	UPROPERTY(EditDefaultsOnly , category = "Inventory")
	TSubclassOf<UInv_ItemDescription> ItemDescriptionClass;
	
	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> ItemDescription;
	
	UInv_ItemDescription* GetItemDescriptionWidget();
	
	FTimerHandle DescriptionTimerHandle;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	float DescriptionTimerDelay = 0.5f;
	
	void SetItemDescriptionSizeAndPosition(UInv_ItemDescription* ItemDescriptionWidget , UCanvasPanel* Canvas);
	
	
	
	UFUNCTION()
	void OnEquippedGridSlotClicked(UInv_EquippedGridSlot* EuippGridSlot , const FGameplayTag& EquipmentTypeTag);
	
	bool CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot , const FGameplayTag& EquipmentTypeTag);
	
	
	UFUNCTION()
	void EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem);
	UInv_EquippedGridSlot* FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem);
	void MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem , UInv_EquippedGridSlot* EquippedGridSlot , UInv_InventoryItem* ItemToEquip);
	void BroadcastSlotClickedChanged(UInv_InventoryItem* ItemToEquip , UInv_InventoryItem* ItemToUnEquip);

}; 

