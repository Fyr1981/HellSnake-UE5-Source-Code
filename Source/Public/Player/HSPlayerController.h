// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "HSPlayerController.generated.h"

UCLASS()
class SNAKEGAME_API AHSPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    FOnPauseInput OnPauseInput;
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    FVector GetLocationUnderCursor();

    virtual void SetupInputComponent() override;

protected:
    virtual void BeginPlay() override;

    virtual void OnPossess(APawn* InPawn) override;

    void InputPause();
};
