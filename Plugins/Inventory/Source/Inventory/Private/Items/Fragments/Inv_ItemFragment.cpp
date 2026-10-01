#include "Items/Fragments/Inv_ItemFragment.h"

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red , FString::Printf(TEXT("Consume Successfully : %f"),HealthAmount));
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue , FString::Printf(TEXT("Consume Successfully : %f"),ManaAmount));
}
