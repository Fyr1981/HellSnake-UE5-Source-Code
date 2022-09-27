// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Improvements/HSBaseImprovement.h"
#include "HSGivenGrenadeImprovement.generated.h"


UCLASS(Blueprintable)
class SNAKEGAME_API UHSGivenGrenadeImprovement : public UHSBaseImprovement
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    TSubclassOf<AHSGrenade> GetGrenadeClass() { return GrenadeClass; }

    virtual void ApplyToSnake(AHSSnakeCharacter* SnakeCharacter) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<AHSGrenade> GrenadeClass = nullptr;
};
