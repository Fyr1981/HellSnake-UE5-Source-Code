// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Placeables/HSPickup.h"
#include "HSSpeedUp.generated.h"

UCLASS()
class SNAKEGAME_API AHSSpeedUp : public AHSPickup
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "BoostParams")
    float GetTimeOfBoost() const { return TimeOfBoost; };
    UFUNCTION(BlueprintCallable, Category = "BoostParams")
    float GetSpeedUpMultiplier() const { return SpeedUpMultiplier; };

private:
    UPROPERTY(EditAnywhere, Category = "BoostParams")
    float TimeOfBoost = 5.0f;
    UPROPERTY(EditAnywhere, Category = "BoostParams")
    float SpeedUpMultiplier = 3.0f;
};
