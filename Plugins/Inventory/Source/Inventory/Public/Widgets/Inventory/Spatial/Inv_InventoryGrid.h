// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_SlottedItem;
class UInv_ItemComponent;
struct FInv_SlotAvailabilityResult;
class UInv_InventoryItem;
class UInv_InventoryComponent;
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
	
	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);



	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent) ;

private:
	
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	
	void ConstructGrid();
	
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* Item);
	FInv_SlotAvailabilityResult HasRoomForItem(const FInv_ItemManifest& Manifest);
	void AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	FVector2D GetDrawSize(const FInv_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UInv_SlottedItem* SlottedItem , const FInv_GridFragment* GridFragment,const FInv_ImageFragment* ImageFragment);
	
	void AddItemAtIndex(UInv_InventoryItem* Item, int32 Index , const bool bStackable , const int32 StackAmount);
	
	void AddSlottedItemToCanvas(const int32 Index , const FInv_GridFragment* GridFragment , UInv_SlottedItem* SlottedItem) const;
	
	void UpdateGridSlots(UInv_InventoryItem* NewItem , const int32 Index , bool bStackableItem , const int32 StackAmount);
	
	UInv_SlottedItem* CreateSlottedItem(
		UInv_InventoryItem* Item ,
		const bool bStackable ,
		const int32 StackAmount , 
		const FInv_GridFragment* GridFragment,
		const FInv_ImageFragment* ImageFragment,
		int32 Index);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),  Category = "Inventory")
	EInv_ItemCategory ItemCategory;
	
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere,Category ="Inventory")
	float TileSize;  //默认值为54
	
	
	bool MatchesCategory(const UInv_InventoryItem* Item) const;
};
