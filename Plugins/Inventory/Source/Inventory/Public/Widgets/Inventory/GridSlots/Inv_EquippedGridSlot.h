
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_GridSlot.h"
#include "Inv_EquippedGridSlot.generated.h"


class UOverlay;
class UInv_EquippedSlottedItem;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked , UInv_EquippedGridSlot* , EuippGridSlot , const FGameplayTag&  , EquipmentTypeTag);


UCLASS()
class INVENTORY_API UInv_EquippedGridSlot : public UInv_GridSlot
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	FEquippedGridSlotClicked OnEquippedGridSlotClicked;
	
	UInv_EquippedSlottedItem* OnItemEquipped(UInv_InventoryItem* Item , const FGameplayTag& EquipTypeTag , float TileSize);
	
	void SetEquippedSlottedItem(UInv_EquippedSlottedItem* SlottedItem){ EquippedSlottedItem = SlottedItem; }
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;
	
	UPROPERTY(EditDefaultsOnly , Category="Inventory" , meta=(Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;
	
	
	UPROPERTY(EditDefaultsOnly , Category="Inventory")
	TSubclassOf<UInv_EquippedSlottedItem> EquippedSlottedItemClass;
	
	UPROPERTY()
	TObjectPtr<UInv_EquippedSlottedItem> EquippedSlottedItem;
	
	
};
