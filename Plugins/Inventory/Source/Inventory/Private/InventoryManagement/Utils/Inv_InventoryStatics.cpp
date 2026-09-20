// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Inv_InventoryStatics.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"

UInv_InventoryComponent* UInv_InventoryStatics::GetInventoryComponent(const APlayerController* OwningPlayerController)
{
	if (!IsValid(OwningPlayerController)) return nullptr;
	
	UInv_InventoryComponent* InventoryComponent = OwningPlayerController->FindComponentByClass<UInv_InventoryComponent>();
	
	return IsValid(InventoryComponent) ? InventoryComponent : nullptr;
}
