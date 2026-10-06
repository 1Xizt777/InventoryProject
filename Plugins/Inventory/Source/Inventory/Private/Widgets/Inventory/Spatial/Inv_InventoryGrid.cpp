
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ConstructGrid();
	
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this , &ThisClass::AddItem);
	InventoryComponent->OnStackChanged.AddDynamic(this , &ThisClass::AddStacks);
	InventoryComponent->OnInventoryMenuToggled.AddDynamic(this , &UInv_InventoryGrid::OnInventoryMenuToggled);
}

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const FVector2D CanvasPosition = UInv_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	
	FVector2D CanvasSize = UInv_WidgetUtils::GetWidgetSize(CanvasPanel);
	
	if (CursorExitedCanvas(CanvasPosition , CanvasSize , MousePosition))	//光标离开Canvas了吗
	{
		return;
	}	
	
	UpdateTileParamerters(CanvasPosition , MousePosition);
}

void UInv_InventoryGrid::SetOwningCanvasPanel(UCanvasPanel* CanvasPan)
{
	OwningCanvasPanel = CanvasPan;
}

void UInv_InventoryGrid::UpdateTileParamerters(const FVector2D CanvasPosition, const FVector2D MousePosition)
{
	
	if (!bMouseWithInCanvas) return;
	
	//把鼠标的屏幕位置，换算成「第几列、第几行」
	const FIntPoint HoveredTileCoordinates = CalculateHoverCoordinates(CanvasPosition , MousePosition);
	
	
	LastTileParameters = TileParameters;
	
	
	TileParameters.TileCoordinates = HoveredTileCoordinates;		//格子坐标
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates , Columns);	//通过格子坐标计算Index
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition , MousePosition);
	
	
	OnTileParametersUpdated(TileParameters);
}


void UInv_InventoryGrid::OnTileParametersUpdated(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	//获取想放置的格子的左上角坐标
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinates ,Dimensions , Parameters.TileQuadrant);
	
	//想放置的格子的左上角索引(手上物品想去哪)
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartingCoordinate , Columns);
	
	
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate , Dimensions);
	
	
	if (CurrentQueryResult.bHasSpace)	//	区域全空
	{
		HighlightSlots(ItemDropIndex , Dimensions);
		return;
	}
	
	//走到这里说明这一帧 bHasSpace = false
	UnHighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);	
	
	
	///只撞到一件物品
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))	
	{
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidItem.Get() , FragmentTags::GridFragment);
		if (!GridFragment) return;
		
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex , GridFragment->GetGridSize() , EInv_GridSlotState::GrayedOut);
	}
}


//拖拽手感核心
FIntPoint UInv_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions,const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;
	
	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EInv_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
		
	case EInv_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;	//如果是偶数，中心在一条缝 需要+1
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
		
	case EInv_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
		
	case EInv_TileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
		 
	default:
		UE_LOG(LogInventory , Warning , TEXT("Inv_InventoryGrid: Invalid Quadrant!"));
		return FIntPoint(-1,-1);
	}
	
	return StartingCoord;
}

FInv_SpaceQueryResuly UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position,const FIntPoint& Dimensions)
{
	FInv_SpaceQueryResuly Result;
	
	if (!IsInGridBound(UInv_WidgetUtils::GetIndexFromPosition(Position,Columns) , Dimensions)) return Result;
	
	Result.bHasSpace = true;
	
	
	TSet<int32> OccupiedUpperLeftIndex;
	UInv_InventoryStatics::ForEach2D(GridSlots ,UInv_WidgetUtils::GetIndexFromPosition(Position,Columns) ,Dimensions , Columns , [&](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndex.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	
	if (OccupiedUpperLeftIndex.Num() == 1)	//物品是单个，可以交换
	{
		const int32 Index = *OccupiedUpperLeftIndex.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();			//本来在那的物品，存进去,后续判断isvalid就是可交换
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();		//本来在那的物品的左上角索引，存进去,后续判断isvalid就是可交换
	}
	
	
	return Result;
}

bool UInv_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPosition, const FVector2D BoundarySize,const FVector2D Location)
{
	bLastMouseWithInCanvas = bMouseWithInCanvas;
	
	bMouseWithInCanvas = UInv_WidgetUtils::IsWithinBounds(BoundaryPosition , BoundarySize , Location);
	
	if (!bMouseWithInCanvas && bLastMouseWithInCanvas)	//上一帧在，这一帧不在
	{
		UnHighlightSlots(LastHighlightedIndex , LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithInCanvas) return;
	
	UnHighlightSlots(LastHighlightedIndex , LastHighlightedDimensions);		// 1.擦旧
	
	UInv_InventoryStatics::ForEach2D(GridSlots , Index , Dimensions, Columns  , [](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();		// 2.画新
	});
	
	LastHighlightedDimensions = Dimensions;		// 3.记住
	LastHighlightedIndex = Index;
}

void UInv_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlots , Index , Dimensions, Columns  , [](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->IsAvailable())	//是否可用
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions,EInv_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex , LastHighlightedDimensions);		//1.擦旧
	
	UInv_InventoryStatics::ForEach2D(GridSlots , Index , Dimensions, Columns  , [State = GridSlotState](UInv_GridSlot* GridSlot)
	{
		switch (State)									//2.画新
		{						
		case EInv_GridSlotState::Occupied: 
			GridSlot->SetOccupiedTexture();
			break;
			
		case EInv_GridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
			
		case EInv_GridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
			
		case EInv_GridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		}
	});
	
	LastHighlightedIndex = Index;						//3.记录
	LastHighlightedDimensions = Dimensions;
}




FIntPoint UInv_InventoryGrid::CalculateHoverCoordinates(const FVector2D CanvasPosition, const FVector2D MousePosition) const
{
	return FIntPoint{static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
					static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y)/ TileSize))}; 
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D CanvasPosition,const FVector2D MousePosition) const
{
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X , TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y , TileSize);	
	
	const bool bIsTop = TileLocalY < TileSize / 2;															
	const bool bIsLeft = TileLocalX < TileSize / 2;
			
	EInv_TileQuadrant HoveredTileQuadrant = EInv_TileQuadrant::None;
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomRight;
	
	return HoveredTileQuadrant;
	
	/*
	  				* 格子内部坐标（0~54）
	   (0,0)                 (27,0)                (54,0)
		┌────────────────────┬────────────────────┐
		│                    │                    │
		│      TopLeft       │      TopRight      │
		│    x < 27, y < 27  │   x ≥ 27, y < 27   │
		│                    │                    │
	  (0,27)─────────────────┼────────────────────┤(54,27)
		│                    │                    │
		│    BottomLeft      │    BottomRight     │
		│   x < 27, y ≥ 27   │   x ≥ 27, y ≥ 27   │
		│                    │                    │
		└────────────────────┴────────────────────┘
		(0,54)              (27,54)               (54,54)
	 */
	

}




void UInv_InventoryGrid::OnSlottedItemClicked(int32 ClickedTileIndex, const FPointerEvent& MouseEvent)
{
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());
	
	check(GridSlots.IsValidIndex(ClickedTileIndex));
	UInv_InventoryItem* ClickInventoryItem = GridSlots[ClickedTileIndex]->GetInventoryItem().Get();
	
	if (!IsValid(ClickInventoryItem)) return;	//图标与格子数据脱钩的孤儿SlottedItem，直接忽略
	
	//只管拖拽
	if (!IsValid(HoverItem) && IsLeftClicked(MouseEvent))	//如果HoverItem还没创建(没在拖拽)，就进入Pickup(创建HoverItem)
	{
		PickUp(ClickInventoryItem , ClickedTileIndex);
		return;
	}
	
	
	if (IsRightClicked(MouseEvent))
	{
		CreateItemPopUp(ClickedTileIndex);
		return;
	}
	
	
	//只管放置Begin//
	if (IsSameStackable(ClickInventoryItem))	//拖拽的物品 与 点击的格内物品 是否是相同的
	{
		const FInv_StackableFragment* StackableFragment = ClickInventoryItem->GetItemManifest().GetFragmentOfType<FInv_StackableFragment>();
		const int32 MaxStackCount = StackableFragment->GetMaxStackSize();
		const int32 ClickedItemStackCount = GridSlots[ClickedTileIndex]->GetStackCount();
		const int32 RoomInClickedSlot = MaxStackCount - ClickedItemStackCount;
		
		const int32 HoverItemStackCount = HoverItem->GetStackCount();
		
		//应不应该交换数量(格子已经没空间了)
		if (ShouldSwapStackCount(HoverItemStackCount, RoomInClickedSlot  , MaxStackCount))		
		{
			SwapStackCount(HoverItemStackCount , ClickedItemStackCount , ClickedTileIndex);
			return;
		}
		
		//应不应该完全消耗所拖拽物品的数量(有位置放，并且能全放完)
		if (ShouldComsumeHoverItemStacks(HoverItemStackCount , RoomInClickedSlot))		
		{
			ComsumeHoverItemStacks(HoverItemStackCount , ClickedItemStackCount , ClickedTileIndex);
			return;
		}
		
		//有位置放，但是放不完
		if (ShouldFillInStack(HoverItemStackCount , RoomInClickedSlot))
		{
			const int32 Remainder = HoverItemStackCount - RoomInClickedSlot;	//拖拽物品填充后所剩数量
			FillInStack(RoomInClickedSlot , Remainder , ClickedTileIndex);
			return;
		}
		
		
		//没空间了
		if (RoomInClickedSlot == 0)
		{
			return;
		}
	}
	SwapWithHoverItem(ClickInventoryItem, ClickedTileIndex);
	// 只管放置End //
	
	
	
}

void UInv_InventoryGrid::CreateItemPopUp(const int32 ClickedTileIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[ClickedTileIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[ClickedTileIndex]->GetItemPopUp())) return;
	
	
	ItemPopUp = CreateWidget<UInv_ItemPopUp>(this , ItemPopUpClass);
	GridSlots[ClickedTileIndex]->SetItemPopUp(ItemPopUp);	//这里面SetGridIndex了
	
	
	if (!OwningCanvasPanel.IsValid()) return;
	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot* CanvasSlot =  UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePosition);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());	
	
	
	//绑定扔下委托
	ItemPopUp->OnDrop.BindDynamic(this , &ThisClass::OnPopMenuDrop);
	
	
	//如果此Item是可堆叠的，才需要Split
	const int32 SliderMax = GridSlots[ClickedTileIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this , &ThisClass::OnPopMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax , FMath::Max(1 , GridSlots[ClickedTileIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}
	
	
	//如果此Item是可消耗的，才需要consume
	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this , &ThisClass::OnPopMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseComsumeButton();
	}
	
}




bool UInv_InventoryGrid::IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem)
{
	if (!IsValid(HoverItem) || !IsValid(ClickedInventoryItem)) return false;	//右键点击时HoverItem可能为空
	
	const bool bIsSameItem = HoverItem->GetInventoryItem() == ClickedInventoryItem;
	const bool bIsStackable = HoverItem->IsStackable();
	const bool bIsSameItemType = HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
	
	return bIsSameItem && bIsStackable && bIsSameItemType;
}

bool UInv_InventoryGrid::ShouldSwapStackCount(const int32 HoverItemStackCount , const int32 RoomInClickedSlot , const int32 MaxStackCount)
{
	return RoomInClickedSlot == 0 && HoverItemStackCount < MaxStackCount;	//想放的格子没有空间 && 拖拽的物品数量 < 该物品格子里能放的最大量
}

void UInv_InventoryGrid::SwapStackCount(const int32 HoverItemStackCount, const int32 ClickedItemStackCount,const int32 ClickedTileIndex)
{
	//1.被点击物品更新数量
	UInv_GridSlot* GridSlot = GridSlots[ClickedTileIndex];
	GridSlot->SetStackCount(HoverItemStackCount);		//1.1 用于数据，算法用
	
	UInv_SlottedItem* SlottedItem = SlottedItems.FindChecked(ClickedTileIndex);
	SlottedItem->UpdateStackCount(HoverItemStackCount);		//1.2 UI显示用
	
	
	//2.所拖拽物品更新数量
	HoverItem->UpdateStackCount(ClickedItemStackCount);
}


bool UInv_InventoryGrid::ShouldComsumeHoverItemStacks(const int32 HoverItemStackCount , const int32 RoomInClickedSlot)
{
	return RoomInClickedSlot >= HoverItemStackCount;
}

void UInv_InventoryGrid::ComsumeHoverItemStacks(const int32 HoverItemStackCount, const int32 ClickedItemStackCount,const int32 ClickedTileIndex)
{
	//1. 新数量 = 已有数量 + 所拖拽物品数量
	const int32 AmountNeedToTransfor = HoverItemStackCount;
	const int32 NewClickedItemStackCount = ClickedItemStackCount + AmountNeedToTransfor;
	
	//2.被点击物品更新数量
	UInv_GridSlot* GridSlot = GridSlots[ClickedTileIndex];
	GridSlot->SetStackCount(NewClickedItemStackCount);		//2.1 用于数据，算法用
	
	UInv_SlottedItem* SlottedItem = SlottedItems.FindChecked(ClickedTileIndex);
	SlottedItem->UpdateStackCount(NewClickedItemStackCount);		//2.2 UI显示用
	
	
	//3.所拖拽物品更新数量
	ClearHoverItem();
	ShowCursor();
	
	
	//4.修复高光
	const FInv_GridFragment* GridFragment = GridSlot->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions = GridFragment->GetGridSize();
	HighlightSlots(ClickedTileIndex , Dimensions);
}

bool UInv_InventoryGrid::ShouldFillInStack(const int32 HoverItemStackCount, const int32 RoomInClickedSlot)
{
	return RoomInClickedSlot < HoverItemStackCount;
}

void UInv_InventoryGrid::FillInStack(const int32 AmountToFill, const int32 Remainder, const int32 ClickedTileIndex)
{
	UInv_GridSlot* GridSlot = GridSlots[ClickedTileIndex];
	UInv_SlottedItem* SlottedItem = SlottedItems.FindChecked(ClickedTileIndex);
	
	//1. 新数量 = 已有数量 + 所拖拽物品数量
	const int32 NewClickedItemStackCount = GridSlot->GetStackCount() + AmountToFill;
	
	//2.被点击物品更新数量
	GridSlot->SetStackCount(NewClickedItemStackCount);		//2.1 用于数据，算法用
	SlottedItem->UpdateStackCount(NewClickedItemStackCount);	//2.2 UI显示用
	
	//3.所拖拽物品更新数量
	HoverItem->UpdateStackCount(Remainder);
}


void UInv_InventoryGrid::SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;
	
	UInv_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool TempIsStackable = HoverItem->IsStackable();

	AssignHoverItem(ClickedInventoryItem , GridIndex , HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem , GridIndex);
	AddItemAtIndex(TempInventoryItem , ItemDropIndex , TempIsStackable , TempStackCount);
	UpdateGridSlots(TempInventoryItem , ItemDropIndex , TempIsStackable , TempStackCount);
	
}




void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;
	
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item);
	
	
	AddItemToIndices(Result, Item);		//一系列UI操作
}


void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;
	
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			// 只更新数字，【不创建】新图标
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);	//更新数量差额给UI
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);			//更新自己的数量
		}
		else
		{
			// 创建图标 + 标记格子
			AddItemAtIndex(Result.Item.Get() , Availability.Index , Result.bStackable , Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get() , Availability.Index , Result.bStackable , Availability.AmountToFill);
		}
	}
}




void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	
	for (const auto& Availablility : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem , Availablility.Index , Result.bStackable , Availablility.AmountToFill);
		UpdateGridSlots(NewItem , Availablility.Index , Result.bStackable , Availablility.AmountToFill);
	}
	
}




void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, int32 Index, const bool bStackable,const int32 StackAmount)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::ImageFragment);
	if (!GridFragment || !ImageFragment) return;
	
	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item , bStackable , StackAmount , GridFragment , ImageFragment , Index);	
	
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	
	//若该下标存在残留旧图标，先移除，避免TMap::Add静默覆盖后旧widget变成孤儿
	if (TObjectPtr<UInv_SlottedItem>* ExistingSlottedItem = SlottedItems.Find(Index))
	{
		if (IsValid(*ExistingSlottedItem))
		{
			(*ExistingSlottedItem)->RemoveFromParent();
		}
	}
	
	SlottedItems.Add(Index, SlottedItem);
}



UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount,const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment , int32 Index)
{
	checkf(IsValid(SlottedItemClass) , TEXT("Inv_InventoryGrid::CreateSlottedItem：SlottedItemClass is not valid!"));
	
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer() ,SlottedItemClass);	//创建单个格子UI
	
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);  //设好图标
	SlottedItem->SetGridIndex(Index);	//从哪个格子下标开始
	SlottedItem->SetIsStackable(bStackable);
	
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;	//如果可堆叠，更新右下角数量标
	SlottedItem->UpdateStackCount(StackUpdateAmount);	
	
	SlottedItem->OnSlottedItemClicked.AddDynamic(this,&ThisClass::OnSlottedItemClicked);
	
	return SlottedItem;
}
void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment)
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);	//图标大小
	SlottedItem->SetImageBrush(Brush);
}
FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;; //计算尺寸（单格里图标能画多大）54 - 5 * 2 = 44
	
	return GridFragment->GetGridSize() * IconTileWidth;  //(1,1) * 44 = (44 , 44)
}
void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment, UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPosWithPadding  = DrawPos + FVector2D(GridFragment->GetGridPadding());	//图标从哪里开始，所以加一份GridPadding
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index , bool bStackableItem , const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));
	
	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);	//仅记左上角那一格的数量
	}
	
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem , FragmentTags::GridFragment);
	if (!GridFragment) return;
	const FIntPoint Dimensions =  GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);	//比如这个物品要占2，3格就是(2,3)
	
	//ForEach2D 从 Index 开始，遍历 Dimensions 大小的矩形，对每格调一次这个 lambda
	UInv_InventoryStatics::ForEach2D(GridSlots,Index,Dimensions,Columns,[&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);		//设定每个格子的左上角格子索引(只有左上角格子有数量)
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);		//	设为不可用
	});
	
}


FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}


FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item , const int32 StackAmountOverride)
{
	return HasRoomForItem(Item->GetItemManifest(),StackAmountOverride);
}


FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest , const int32 StackAmountOverride)
{
	FInv_SlotAvailabilityResult Result;
	
	// Determine if the item is stackable.
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Determine how many stacks to add.
	const int32 MaxStackSize =  Result.bStackable ? StackableFragment->GetMaxStackSize() : 1;	//单个能叠多少个
	int32 AmountToFill = Result.bStackable ? StackableFragment->GetStackCount() : 1;		//拾取一次有几个	
	if (StackAmountOverride != -1 && Result.bStackable)
	{
		AmountToFill = StackAmountOverride;
	}
	
	
	TSet<int32> CheckedIndices;	//本次已确定要占的格子
	
	
	// For each Grid Slot:
	for (const auto& GridSlot : GridSlots)
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill == 0) break;
		
		
		// Is this index claimed yet?
		if (IsIndexClaimed(CheckedIndices , GridSlot->GetTileIndex())) continue;
		
		
		//Is the Item in Grid bounds?
		if (!IsInGridBound(GridSlot->GetTileIndex() , GetItemDimensions(Manifest))) continue;
		
		
		// Can the item fit here? (i.e. is it out of grid bounds?)
		TSet<int32> TentativelyClaimed;		//试探性预定哈希表
		if (!HasRoomAtIndex(GridSlot , GetItemDimensions(Manifest) , CheckedIndices , TentativelyClaimed , Manifest.GetItemType() , MaxStackSize))
		{
			continue;
		}
		
		
		// How much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable  , MaxStackSize , AmountToFill , GridSlot);
		if (AmountToFillInSlot == 0) continue; 
		
		
		CheckedIndices.Append(TentativelyClaimed);
		
		
		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		
		//直接构造一个FInv_SlotAvailability塞进去
		Result.SlotAvailabilities.Emplace(		
			FInv_SlotAvailability{
			HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)}
				);
		

		AmountToFill -= AmountToFillInSlot;
		
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0 ) return Result;
	}
	
	
	return Result;
}
bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, int32 Index) const
{
	return CheckedIndices.Contains(Index);
}
												
bool UInv_InventoryGrid::IsInGridBound(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	
	const int32 EndColumn = StartIndex % Columns + ItemDimensions.X;	//起始列号 + 物品宽
	const int32 EndRow = StartIndex / Columns + ItemDimensions.Y;	 	//起始行号 + 物品高
	
	return EndColumn <= Columns && EndRow <= Rows;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions ,
const TSet<int32>& CheckedIndices , TSet<int32>& OutTentativelyClaimed , const FGameplayTag& ItemType , const int32 MaxStackSize)
{
	bool bHasRoomAtIndex = true;
	// Is there room at this index? (i.e. are there other items in the way?)
	UInv_InventoryStatics::ForEach2D(GridSlots , GridSlot->GetTileIndex() , Dimensions , Columns , [&](const UInv_GridSlot* SubGridSlot)
	{
		 if (CheckSlotConstraints(GridSlot , SubGridSlot , CheckedIndices,OutTentativelyClaimed , ItemType , MaxStackSize))
		 {
			 OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		 }
		 else
		 {
			 bHasRoomAtIndex = false;
		 }
	});
	
	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot , const UInv_GridSlot* SubGridSlot ,
	const TSet<int32>& CheckedIndcies  , TSet<int32>& OutTentativelyClaimed , const FGameplayTag& ItemType , const int32 MaxStackSize) const
{
	//格子索引已经被占了吗
	if (IsIndexClaimed(CheckedIndcies , SubGridSlot->GetTileIndex())) return false;
	
	
	//格子里面有Item了吗
	if (!HasValidItem(SubGridSlot))	
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		return true;
	}
	
	
	//这个格子是左上角的格子吗
	if (!IsUpperLeftSlot(GridSlot , SubGridSlot)) return false;
	
	
	//该物品可堆叠吗
	UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	
	 
	//这个物品和我们想要添加的物品是同一类型的吗
	if (!DoesItemTypeMatch(SubItem , ItemType)) return false;
	
	
	//如果可堆叠，该槽位是否已达到最大堆叠数量？
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	
	return true;
}



bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetTileIndex() == GridSlot->GetTileIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsLeftClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

bool UInv_InventoryGrid::IsRightClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem , GridIndex  , GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}
void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem , const int32 GridIndex , const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 1);
}


void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer() , HoverItemClass);
	}
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::ImageFragment);
	if (!GridFragment || !ImageFragment) return;
	
	const FVector2D DrawSize = GetDrawSize(GridFragment);
	
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);
	
	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());
	
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default , HoverItem);
}

void UInv_InventoryGrid::OnHide()
{
	PutHoverItemBack();
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!(GridFragment)) return;
	
	//遍历每一格清空数据，回到没有放置物品状态
	UInv_InventoryStatics::ForEach2D(GridSlots , GridIndex , GridFragment->GetGridSize() , Columns , [](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
		GridSlot->SetInventoryItem(nullptr);
	});
	
	//从TMap获取那个SlottedItem
	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex , FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();		//真正去掉SlottedItem的图标等
	}
	
}



int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize,
                                                     const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);		//还能放下几个
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;		//单次拾取数量和格子剩余容量谁小
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();	//只有左上角那一格有具体数量
	}
	return CurrentSlotStackCount;
}




void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);
	
	
	for (int32 Row = 0 ; Row < Rows; ++Row)	//行
	{
		for (int32 Column = 0; Column < Columns; ++Column)	//列
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);		//引擎建好 WidgetSlot 并绑定
			
			const FIntPoint TilePosition(Column,Row);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));
			
			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition*TileSize);	//  (0,1)*54 = (0,54)  
															//  (1,2)*54 = (54,108)
			GridSlots.Add(GridSlot);
			
			GridSlot->GridSlotClicked.AddDynamic(this , &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnHovered); 
		}
	}
}

	//此函数处理拖拽后的放置
void UInv_InventoryGrid::OnGridSlotClicked(int32 Index, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;		//不是在拖拽直接return,这里是拖拽后的放置
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;
	
	
	//交换路线
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))	
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex , MouseEvent);
		return;
	}
	
	
	//直接放置路线：必须整个脚印区域全空（bHasSpace由CheckHoverPosition保证），否则会覆盖其他物品的格子数据
	if (CurrentQueryResult.bHasSpace)
	{
		PutDownOnIndex(ItemDropIndex);
	}
	
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	if (!IsValid(HoverItem)) return;
	
	AddItemAtIndex(HoverItem->GetInventoryItem() , Index , HoverItem->IsStackable() , HoverItem->GetStackCount());		//创建SlottedItem，处理好image等
	UpdateGridSlots(HoverItem->GetInventoryItem() , Index , HoverItem->IsStackable() , HoverItem->GetStackCount());	//更新数据
	ClearHoverItem();
}

void UInv_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	
	HoverItem->SetImageBrush(FSlateNoResource());
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;
	
	//显示鼠标光标
	ShowCursor();
}


void UInv_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default , GetVisibleCursorWidget());
}

void UInv_InventoryGrid::HiddenCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer() , VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer() , HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}



void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;		//在拖拽直接return，在拖拽归Tick管
	
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	
	if (GridSlot->IsAvailable())	//是否有物品（SlottedItem是否有效）
	{
		GridSlot->SetOccupiedTexture();
	}
		
}

void UInv_InventoryGrid::OnGridSlotUnHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;		//在拖拽直接return，在拖拽归Tick管
	
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	
	if (GridSlot->IsAvailable())	//是否有物品（SlottedItem是否有效）
	{
		GridSlot->SetUnoccupiedTexture();
	}
}


void UInv_InventoryGrid::OnPopMenuSplit(int32 SplitAmount, int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (!RightClickedItem->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetUpperLeftIndex();		//仅有左上角那个记数量
	
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	UInv_SlottedItem* UpperLeftSlottedItem = SlottedItems.FindChecked(UpperLeftIndex);
	if (!IsValid(UpperLeftGridSlot) || !IsValid(UpperLeftSlottedItem)) return;
	
	
	const int32 NewStackCount = UpperLeftGridSlot->GetStackCount()-SplitAmount;
	UpperLeftGridSlot->SetStackCount(NewStackCount);	//更新数量
	UpperLeftSlottedItem->UpdateStackCount(NewStackCount);	//UI更新数量
	
	AssignHoverItem(RightClickedItem , UpperLeftIndex , UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UInv_InventoryGrid::OnPopMenuDrop(int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	
	PickUp(RightClickedItem , GridIndex);	//PickUp里将要丢下的物品创建为HoverItem了
	DropItem();
}
void UInv_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(HoverItem->GetInventoryItem())) return;
	
	//告诉服务器丢下
	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem() , HoverItem->GetStackCount());
	
	ClearHoverItem();
	ShowCursor();
}

bool UInv_InventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

UInv_HoverItem* UInv_InventoryGrid::GetHoverItem() const
{
	return HoverItem;
}


void UInv_InventoryGrid::OnPopMenuConsume(int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	
	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	UInv_SlottedItem* UpperLeftSlottedItem = SlottedItems.FindChecked(UpperLeftIndex);
	if (!IsValid(UpperLeftGridSlot) || !IsValid(UpperLeftSlottedItem)) return;
	
	
	const int32 NewStackCount = UpperLeftGridSlot->GetStackCount() - 1;
	
	//更新数量
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	UpperLeftSlottedItem->UpdateStackCount(NewStackCount);
	
	InventoryComponent->Server_ConsumeItem(RightClickedItem);
	
	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(RightClickedItem , GridIndex);
	}
	
	
}

void UInv_InventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem) ||  !IsValid(HoverItem->GetInventoryItem())) return;
	
	FInv_SlotAvailabilityResult Result = HasRoomForItem(HoverItem->GetInventoryItem() , HoverItem->GetStackCount());
	
	AddStacks(Result);
	ClearHoverItem();
}

void UInv_InventoryGrid::OnInventoryMenuToggled(bool bOpen)
{
	if (!bOpen)
	{
		PutHoverItemBack();
	}
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

