// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Inv_InventoryStatics.generated.h"


class UInv_InventoryComponent;
class UInv_ItemComponent;


UCLASS()
class INVENTORY_API UInv_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_InventoryComponent* GetInventoryComponent(const APlayerController* OwningPlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EInv_ItemCategory GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComponent);
	
	template<typename T, typename FuncT>		
	static void ForEach2D(TArray<T>& Array , int32 Index , FIntPoint Range2D , int32 GridColumns , const FuncT& Function);
};


//从一个起始格出发，遍历一个矩形区域里的所有格子，对每个格子调用一次回调
template <typename T, typename FuncT>
void UInv_InventoryStatics::ForEach2D(TArray<T>& Array, int32 Index, FIntPoint Range2D, int32 GridColumns,const FuncT& Function)
{
	for (int32 j = 0 ; j < Range2D.Y ; ++j)
	{
		for (int32 i = 0 ; i < Range2D.X ; ++i)
		{
			const FIntPoint Coordinates = UInv_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i , j);
			const int32 TileIndex = UInv_WidgetUtils::GetIndexFromPosition(Coordinates , GridColumns);
			
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
