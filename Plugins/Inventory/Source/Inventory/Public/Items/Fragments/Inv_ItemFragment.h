#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RHITransientResourceAllocator.h"

#include "Inv_ItemFragment.generated.h"


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
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
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

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1, 1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.f};
	
};


USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_ItemFragment
{
	GENERATED_BODY()
	
public:
	
	UTexture2D* GetIcon() const { return Icon; }
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimension{44.f,44.f};
};


