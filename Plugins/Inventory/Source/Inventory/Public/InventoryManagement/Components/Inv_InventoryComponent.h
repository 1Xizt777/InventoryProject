#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "Inv_InventoryComponent.generated.h"


struct FInv_SlotAvailabilityResult;
struct FInv_InventoryFastArray;
class UInv_InventoryItem;
class UInv_InventoryBase;
class UInv_ItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChanged , UInv_InventoryItem* , Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChanged , const FInv_SlotAvailabilityResult&  ,Result);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class INVENTORY_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInv_InventoryComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ToggleInventoryMenu();
	
	FInventoryItemChanged OnItemAdded;
	FInventoryItemChanged OnItemRemoved;
	FNoRoomInInventory NoRoomInInventory;
	FStackChanged OnStackChanged;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly , Category = "Inventory")
	void TryAddItem(UInv_ItemComponent* ItemComponent);
	
	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UInv_ItemComponent* ItemComponent , int32 StackCount);
	
	UFUNCTION(Server, Reliable)
	void Server_AddStackToItem(UInv_ItemComponent* ItemComponent, int32 StackCount , int32 Remainder);
	
	void AddRepSubObj(UObject* SubObj);
	
	
	
protected:

	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<APlayerController> OwningController;
	
	void ConstructInventory();	//Create InventoryMenu Widget
	
	UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryBase> InventoryMenu;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInv_InventoryBase> InventoryMenuClass;
	
	
	bool bInventoryMenuOpen;
	
	void OpenInventoryMenu();
	void CloseInventoryMenu();
};
