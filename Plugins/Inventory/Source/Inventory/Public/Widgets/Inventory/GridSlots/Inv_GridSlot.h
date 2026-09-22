#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_InventoryItem;

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
	
	int32 GetTileIndex() const {return TileIndex;}
	void SetTileIndex(int32 Index){TileIndex = Index;}
	
	EInv_GridSlotState GetGridSlotState() const {return GridSlotState;}
	
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInv_InventoryItem* Item);
	
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }
	
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }
	
	bool GetAvailable() const { return bAvailable; }
	void SetAvailable(bool bIsAvailable) { bAvailable = bIsAvailable; }
	
	
	void SetUnoccupiedTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();
private:
	
	int32 TileIndex;
	int32 StackCount;
	int32 UpperLeftIndex = {INDEX_NONE};
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bAvailable;
	
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
	
};
