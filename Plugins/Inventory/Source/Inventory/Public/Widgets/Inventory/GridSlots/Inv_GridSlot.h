#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_ItemPopUp;
class UInv_InventoryItem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent , int32 , Index , const FPointerEvent&  , MouseEvent);

UENUM()
enum class EInv_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};


class UImage;

UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	
	int32 GetTileIndex() const {return TileIndex;}
	void SetTileIndex(int32 Index){TileIndex = Index;}
	
	EInv_GridSlotState GetGridSlotState() const {return GridSlotState;}
	
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInv_InventoryItem* Item);
	
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }
	
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }
	
	bool IsAvailable() const { return bAvailable; }
	void SetAvailable(bool bIsAvailable) { bAvailable = bIsAvailable; }
	
	UInv_ItemPopUp* GetItemPopUp() const;
	void SetItemPopUp(UInv_ItemPopUp* ItemPop);
	
	
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();
	
	
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;
	FGridSlotEvent GridSlotClicked;

private:
	
	int32 TileIndex{INDEX_NONE};
	
	//此StackCount是给算法用的，
	int32 StackCount{0};
	
	int32 UpperLeftIndex = {INDEX_NONE};
	
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	TWeakObjectPtr<UInv_ItemPopUp> ItemPopUp;
	
	//默认是可用的
	bool bAvailable{true};
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditDefaultsOnly , Category = "Inventory")
	FSlateBrush Brush_GrayedOut;
	
	EInv_GridSlotState GridSlotState;
	
	UFUNCTION()
	void OnItemPopDestruct(UUserWidget* Menu);
};
