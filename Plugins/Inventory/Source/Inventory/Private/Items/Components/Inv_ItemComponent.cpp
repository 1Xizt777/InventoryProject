#include "Items/Components/Inv_ItemComponent.h"


UInv_ItemComponent::UInv_ItemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	PickupMessage = FString("E - Pick Up");
}


