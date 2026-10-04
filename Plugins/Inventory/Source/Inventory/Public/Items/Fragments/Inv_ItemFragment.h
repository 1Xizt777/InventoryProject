#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemFragment.generated.h"

class UInv_CompositeBase;
class APlayerController;

USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	
	//！如果类声明了析构函数（哪怕是空的 {}），编译器就不再自动生成移动构造和移动赋值，所以需要手动写！//
	
	FInv_ItemFragment() {}		//默认构造
	
	FInv_ItemFragment(const FInv_ItemFragment&) = default;		//拷贝构造
	FInv_ItemFragment& operator=(const FInv_ItemFragment&) = default;		//拷贝赋值
	
	FInv_ItemFragment(FInv_ItemFragment&&) = default;		//移动构造
	FInv_ItemFragment& operator=(FInv_ItemFragment&&) = default;	//移动赋值
			
	virtual ~FInv_ItemFragment() {}		//虚析构	
	
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
	
	virtual void Manifest() {}	//Fragment的Manifest，注意区分FInv_Manifest的Mainfest
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory" , meta=(Categories = "FragmentTags"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};







//此结构体‘FInv_InventoryItemFragment’用于集成到小部件上
class UInv_CompositeBase;
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()
	
public:
	
	virtual void Assimilate(UInv_CompositeBase* Composite) const;
	
protected:
	
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
	
};






USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }

private:

	//该物品占几格
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1, 1};

	//图标间隔
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.f};
	
};







USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
public:
	
	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const;
	
private:
	
	//该物品图标
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};
	
	//图标大小
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{44.f,44.f};
};

USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
public:
	void SetText(const FText& Text) {FragmentText = Text;};
	FText GetText() const { return FragmentText; }
	
	virtual void Assimilate(UInv_CompositeBase* Composite) const;
	
private:
	
	UPROPERTY(EditAnywhere , Category = "Inventory")
	FText FragmentText;
};

USTRUCT(BlueprintType)
struct FInv_LabelNumebrFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
public:
	
	virtual void Manifest() override;	//随机Value出来
	
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	
	//仅第一次生成物品的时候会随机Value，随后保持一个值
	bool bRandomizeOnManifest{true};
	
	float GetValue() const { return Value; }
private:
	
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label;
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MaxValue{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MinValue{0.f};
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{false};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
};











USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	
public:
	
	int32 GetMaxStackSize() const { return MaxStackSize; }
	
	
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }
	
private:
	
	//单格最多能叠多少个
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 MaxStackSize{1};	
	
	//拾取一次给几个
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 StackCount{1};	
};








USTRUCT(BlueprintType)
struct FInv_ConsumeModifier : public FInv_LabelNumebrFragment
{
	GENERATED_BODY()
	
	virtual void OnConsume(APlayerController* PC) {}

};



USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC);	
	
	virtual void Assimilate(UInv_CompositeBase* Composite) const;
	
	virtual void Manifest() override;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory" , meta =(ExcludeBaseStruct))	
	TArray<TInstancedStruct<FInv_ConsumeModifier>> ConsumeModifiers;
	
};





USTRUCT(BlueprintType)
struct FInv_HealthPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()
	
	
	virtual void OnConsume(APlayerController* PC) override;
};


USTRUCT(BlueprintType)
struct FInv_ManaPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()

	
	virtual void OnConsume(APlayerController* PC) override;
};