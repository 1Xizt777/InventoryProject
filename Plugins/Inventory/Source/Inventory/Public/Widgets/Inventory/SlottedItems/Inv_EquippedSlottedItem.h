
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_SlottedItem.h"
#include "Inv_EquippedSlottedItem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked , UInv_EquippedSlottedItem* , EquippedSlottedItem);

UCLASS()
class INVENTORY_API UInv_EquippedSlottedItem : public UInv_SlottedItem
{
	GENERATED_BODY()
	
public:
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	void SetEquipmentTypeTag(FGameplayTag TypeTag){EquipmentTypeTag = TypeTag;}
	FGameplayTag GetEquipmentTypeTag(){return EquipmentTypeTag;}
	
	FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;
	
	
private:
	
	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
