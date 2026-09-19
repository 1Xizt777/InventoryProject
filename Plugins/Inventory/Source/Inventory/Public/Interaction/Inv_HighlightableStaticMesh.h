// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Highlightable.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_HighlightableStaticMesh.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class INVENTORY_API UInv_HighlightableStaticMesh : public UStaticMeshComponent , public IInv_Highlightable
{
	GENERATED_BODY()
	
public:
		
	virtual void Hightlight_Implementation() override;
	virtual void UnHightlight_Implementation() override;
	
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
};
