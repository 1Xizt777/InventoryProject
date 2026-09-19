#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inv_PlayerController.generated.h"

class UHUDWidget;
class UInputMappingContext;
class UInputAction;

UCLASS()
class INVENTORY_API AInv_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AInv_PlayerController();
	virtual void Tick(float DeltaTime) override;
	
protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	void PrimaryInteract();
	
	void CreateHUDWidget();
	
	void TraceForItem();
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UHUDWidget> HUDWidget;
	
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	double TraceLength;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> ItemTraceChannel;

	TWeakObjectPtr<AActor> ThisActor;
	TWeakObjectPtr<AActor> LastActor;
};
