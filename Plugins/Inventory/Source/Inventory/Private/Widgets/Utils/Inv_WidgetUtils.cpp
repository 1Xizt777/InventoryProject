

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

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize,const FVector2D& MousePos)
{
	FVector2D ClampedWidgetPosition = MousePos;
	
	if (MousePos.X + WidgetSize.X > Boundary.X)
	{
		ClampedWidgetPosition.X = Boundary.X - WidgetSize.X;	
		/*
		 *坐标系里 X 是控件的左边缘位置，WidgetSize.X 是宽度，所以：
		 *	1.     控件右边缘 = X + WidgetSize.X
		 *	2.     边界右边缘 = Boundary.X
		 *要求"控件完全待在边界内" → 右边缘 ≤ Boundary.X，即：
		 *	3.     X ≤ Boundary.X - WidgetSize.X
		*/		
	}
	if (MousePos.X < 0.f)
	{
		ClampedWidgetPosition.X = 0.f;
	}
	
	
	if (MousePos.Y + WidgetSize.Y > Boundary.Y)
	{
		ClampedWidgetPosition.Y = Boundary.Y - WidgetSize.Y;
	}
	if (MousePos.Y < 0.f)
	{
		ClampedWidgetPosition.Y = 0.f;
	}
	
	return ClampedWidgetPosition;
}
