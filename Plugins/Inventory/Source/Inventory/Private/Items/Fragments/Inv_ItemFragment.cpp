

#include "Items/Fragments/Inv_ItemFragment.h"

#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"


void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}


bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	//传进来的Composite小部件里的FragmentTag是不是和此结构体Tag一样
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());	
}


void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_Image* LeafImageWidget = Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(LeafImageWidget)) return;
		
	LeafImageWidget->SetImage(Icon);
	LeafImageWidget->SetBoxSize(IconDimensions);
	LeafImageWidget->SetImageSize(IconDimensions);
}

void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_Text* LeafTextWidget = Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafTextWidget)) return;
	
	LeafTextWidget->SetText(FragmentText);
}

void FInv_LabelNumebrFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	
	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(MinValue , MaxValue);
	}
	bRandomizeOnManifest = false;
}

void FInv_LabelNumebrFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_LabeledValue* LabeledValueWidget = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValueWidget)) return;
	
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	
	LabeledValueWidget->SetText_Label(Text_Label ,bCollapseLabel);
	LabeledValueWidget->SetText_Value(FText::AsNumber(Value,&Options),bCollapseValue);
	
}





void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnConsume(PC);
	}
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	for (const auto& Modifier : ConsumeModifiers)
	{
		const auto& ModifierRef = Modifier.Get();
		ModifierRef.Assimilate(Composite);
	}
	
}

void FInv_ConsumableFragment::Manifest()
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.Manifest();
	}
}


void FInv_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnEquip(PC);
	}
}

void FInv_EquipmentFragment::OnUnEquip(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnUnEquip(PC);
	}
}

void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : EquipModifiers)
	{
		const auto& ModifierRef = Modifier.Get();
		ModifierRef.Assimilate(Composite);
	}
}


void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red , FString::Printf(TEXT("Consume Successfully : %f"),GetValue()));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue , FString::Printf(TEXT("Consume Successfully : %f"),GetValue()));
}



void FInv_StrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Orange , FString::Printf(TEXT("Equip Successfully , Strength Increased : %f"),GetValue()));
}

void FInv_StrengthModifier::OnUnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Orange , FString::Printf(TEXT("Unequip Successfully , Strength Decreased : %f"),GetValue()));
}
;
