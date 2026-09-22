#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inv_WidgetUtils.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_WidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable , Category = "Inventory")
	static int32 GetIndexFromPosition(const FIntPoint& Position , const int32 Columns);
	
	UFUNCTION(BlueprintCallable , Category = "Inventory") 
	static FIntPoint GetPositionFromIndex(const int32 Index , const int32 Columns);
	
	template<typename T, typename FuncT>		
	static void ForEach2D(TArray<T>& Array , int32 Index , FIntPoint Range2D , int32 GridColumns , const FuncT& Function);
};


//从一个起始格出发，遍历一个矩形区域里的所有格子，对每个格子调用一次回调
template <typename T, typename FuncT>
void UInv_WidgetUtils::ForEach2D(TArray<T>& Array, int32 Index, FIntPoint Range2D, int32 GridColumns,const FuncT& Function)
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
