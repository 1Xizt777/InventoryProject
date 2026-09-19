// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_InventoryGrid.generated.h"

class UCanvasPanel;
class UInv_GridSlot;
enum class EInv_ItemCategory : uint8;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	
private:
	
	void ConstructGrid();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),  Category = "Inventory")
	EInv_ItemCategory ItemCategory;
	
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	float TileSize;
};
