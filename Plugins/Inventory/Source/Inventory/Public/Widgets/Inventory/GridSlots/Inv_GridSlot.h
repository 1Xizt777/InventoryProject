#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"


class UImage;

UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	int32 GetTileIndex() const {return TileIndex;}
	void SetTileIndex(int32 Index){TileIndex = Index;}

private:
	
	int32 TileIndex;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
};
