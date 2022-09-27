// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/PlayerState.h"
#include "HSPlayerState.generated.h"

class AHSSnakeCharacter;
UCLASS()
class SNAKEGAME_API AHSPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AHSPlayerState();

    UPROPERTY(BlueprintAssignable)
    FOnIncrementScore OnIncrementScore;

protected:
    virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnNewPawn(APawn* Pawn);

    UFUNCTION()
    void OnSnakeGrowth(int32 Value);

    virtual void OnRep_Score() override;
};
