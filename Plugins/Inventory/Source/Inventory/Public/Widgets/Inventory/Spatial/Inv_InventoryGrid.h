// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_SpatialInventory;
class UInv_ItemPopUp;
class UInv_HoverItem;
class UInv_SlottedItem;
class UInv_ItemComponent;
struct FInv_SlotAvailabilityResult;
class UInv_InventoryItem;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;
enum class EInv_ItemCategory : uint8;
enum class EInv_GridSlotState : uint8;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetOwningCanvasPanel(UCanvasPanel* CanvasPan);
	
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	
	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);

	void DropItem();
	
	void ShowCursor();
	void HiddenCursor();

	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent) ;

private:
	
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel; 
	
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
	
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices , int32 Index) const;
	
	bool HasRoomAtIndex(const UInv_GridSlot * GridSlot ,
		const FIntPoint& Dimensions , 
		const TSet<int32>& CheckedIndices ,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType ,
		const int32 MaxStackSize);
	 
	
	bool CheckSlotConstraints(const UInv_GridSlot* GridSlot ,
		const UInv_GridSlot* SubGridSlot ,
		const TSet<int32>& CheckedIndcies ,
		TSet<int32>& OutTentativelyClaimed ,
		const FGameplayTag& ItemType ,
		const int32 MaxStackSize) const;
	
	
	//获取该物品尺寸(列，行) -> (2,3)/(1,1)
	FIntPoint GetItemDimensions(const FInv_ItemManifest& Manifest)const;
	
	
	bool HasValidItem(const UInv_GridSlot* GridSlot) const ;
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot , const UInv_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem , const FGameplayTag& ItemType) const;
	
	bool IsInGridBound(const int32 StartIndex , const FIntPoint& ItemDimensions) const;
	
	int32 DetermineFillAmountForSlot(const bool bStackable , const int32 MaxStackSize , const int32 AmountToFill , const UInv_GridSlot* GridSlot) const ;
	int32 GetStackAmount(const UInv_GridSlot* GridSlot) const;
	
	UFUNCTION()
	void AddStacks(const FInv_SlotAvailabilityResult& Result);
	
	UFUNCTION()
	void OnSlottedItemClicked(int32  ClickedTileIndex , const FPointerEvent& MouseEvent);
	
	bool IsLeftClicked(const FPointerEvent& MouseEvent) const;
	bool IsRightClicked(const FPointerEvent& MouseEvent) const;
	void PickUp(UInv_InventoryItem* ClickedInventoryItem , const int32 GridIndex);
	void AssignHoverItem(UInv_InventoryItem* InventoryItem);
	void AssignHoverItem(UInv_InventoryItem* InventoryItem , const int32 GridIndex , const int32 PreviousGridIndex);
	void RemoveItemFromGrid(UInv_InventoryItem* InventoryItem , const int32 GridIndex);
	
	void UpdateTileParamerters(const FVector2D CanvasPosition , const FVector2D MousePosition);
	FIntPoint CalculateHoverCoordinates(const FVector2D CanvasPosition , const FVector2D MousePosition) const;
	EInv_TileQuadrant CalculateTileQuadrant(const FVector2D CanvasPosition , const FVector2D MousePosition) const;
	void OnTileParametersUpdated(const FInv_TileParameters& Parameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate , const FIntPoint& Dimensions , const EInv_TileQuadrant Quadrant) const;
	FInv_SpaceQueryResuly CheckHoverPosition(const FIntPoint& Position , const FIntPoint& Dimensions);
	bool CursorExitedCanvas(const FVector2D& BoundaryPosition , const FVector2D BoundarySize , const FVector2D Location);
	
	void HighlightSlots(const int32 Index , const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index , const FIntPoint& Dimensions);
	
	void ChangeHoverType(const int32 Index , const FIntPoint& Dimensions , EInv_GridSlotState  GridSlotState);
	
	UFUNCTION()
	void OnGridSlotClicked(int32 Index , const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex , const FPointerEvent& MouseEvent);
	
	UFUNCTION()
	void OnGridSlotUnHovered(int32 GridIndex , const FPointerEvent& MouseEvent);
	
	void PutDownOnIndex(const int32 Index);
	void ClearHoverItem();
	
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	
	
	bool IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem);
	
	void SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem , const int32 GridIndex);
	
	bool ShouldSwapStackCount(const int32 HoverItemStackCount , const int32 RoomInClickedSlot , const int32 MaxStackCount);
	void SwapStackCount(const int32 HoverItemStackCount , const int32 ClickedItemStackCount , const int32 ClickedTileIndex);
	
	bool ShouldComsumeHoverItemStacks(const int32 HoverItemStackCount , const int32 RoomInClickedSlot);
	void ComsumeHoverItemStacks(const int32 HoverItemStackCount , const int32 ClickedItemStackCount , const int32 ClickedTileIndex);
	
	bool ShouldFillInStack(const int32 HoverItemStackCount , const int32 RoomInClickedSlot);
	void FillInStack(const int32 AmountToFill , const int32 Remainder , const int32 ClickedTileIndex);
	
	void CreateItemPopUp(const int32 ClickedTileIndex);
	

	
	UFUNCTION()
	void OnPopMenuSplit(int32 SplitAmount , int32 GridIndex);
	
	UFUNCTION()
	void OnPopMenuDrop(int32 GridIndex);
	
	UFUNCTION()
	void OnPopMenuConsume(int32 GridIndex);
	
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	TSubclassOf<UInv_ItemPopUp> ItemPopUpClass;
	
	UPROPERTY()
	TObjectPtr<UInv_ItemPopUp> ItemPopUp;
	
	UPROPERTY(EditAnywhere , Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;
	
	UPROPERTY(EditAnywhere , Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),  Category = "Inventory")
	EInv_ItemCategory ItemCategory;	//这页GridSlot的类型
	
	UPROPERTY()
	TMap<int32 , TObjectPtr<UInv_SlottedItem>>SlottedItems;
	
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
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	TSubclassOf<UInv_HoverItem> HoverItemClass;
	
	UPROPERTY()
	TObjectPtr<UInv_HoverItem> HoverItem;
	
	FInv_TileParameters TileParameters;
	FInv_TileParameters LastTileParameters;
	
	//手上物品想去哪
	int32 ItemDropIndex{INDEX_NONE};
	
	FInv_SpaceQueryResuly CurrentQueryResult;
	
	
	bool bMouseWithInCanvas{false};
	bool bLastMouseWithInCanvas{false};
	
	int32 LastHighlightedIndex{INDEX_NONE};
	FIntPoint LastHighlightedDimensions;
};
