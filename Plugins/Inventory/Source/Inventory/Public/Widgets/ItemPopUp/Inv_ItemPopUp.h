// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

/**
 *	此widget在背包界面右键物品的时候出现
 */

class UButton;
class USlider;
class UTextBlock;
class USizeBox;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit , int32 , SplitAmount , int32 , GridIndex);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop , int32 , Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume , int32 , Index);

UCLASS()
class INVENTORY_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	
	FPopUpMenuSplit OnSplit;
	FPopUpMenuConsume OnConsume;
	FPopUpMenuDrop OnDrop;
	
	int32 GetSplitAmount() const;
	void CollapseSplitButton() const;
	void CollapseComsumeButton() const;
	void SetSliderParams(const int32 MaxValue , const int32 Value)const;
	FVector2D GetBoxSize()const;
	
	void SetGridIndex(int32 Index){GridIndex = Index;};
	int32 GetGridIndex() const {return GridIndex;};
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	int32 GridIndex{INDEX_NONE};
	
	UFUNCTION()
	void SplitButtonClicked();

	UFUNCTION()
	void DropButtonClicked();

	UFUNCTION()
	void ConsumeButtonClicked();

	UFUNCTION()
	void SliderValueChanged(float Value);
	
};
