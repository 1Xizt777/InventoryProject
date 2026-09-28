

#include "Widgets/Utils/Inv_WidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

FVector2D UInv_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D LocalTopLeft = USlateBlueprintLibrary::GetLocalTopLeft(Geometry);		//获取控件自身的局部原点
	
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry , LocalTopLeft , PixelPosition , ViewportPosition);	//将局部坐标转换为视口坐标
								//PixelPosition 从游戏窗口开始量
	return ViewportPosition;	//ViewportPosition 从游戏视口开始量
}

FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UInv_WidgetUtils::IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize,const FVector2D& MousePos)
{
	return MousePos.X >= BoundaryPos.X && MousePos.X <= (BoundaryPos.X + WidgetSize.X) && 
		MousePos.Y >= BoundaryPos.Y && MousePos.Y <= (BoundaryPos.Y + WidgetSize.Y);
}

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;	// Column + Row * Columns
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);		//（列 ，行）
}
