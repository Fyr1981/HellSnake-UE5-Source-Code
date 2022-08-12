// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/PlayerState.h"
#include "HSPlayerState.generated.h"

UCLASS()
class SNAKEGAME_API AHSPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnImprovement OnImprovement;

    void IncrementScore();
};
