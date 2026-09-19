// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Inv_HighlightableStaticMesh.h"

void UInv_HighlightableStaticMesh::Hightlight_Implementation()
{
	SetOverlayMaterial(HighlightMaterial);	
}

void UInv_HighlightableStaticMesh::UnHightlight_Implementation()
{
	SetOverlayMaterial(nullptr);	
}
