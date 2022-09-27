// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Improvements/HSBaseImprovement.h"
#include "HSTempImmortalityImprovement.generated.h"

UCLASS(Blueprintable)
class SNAKEGAME_API UHSTempImmortalityImprovement : public UHSBaseImprovement
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    float GetTimeOfBoost() { return TimeOfBoost; }

    UFUNCTION(BlueprintCallable)
    float GetSpeedUpMultiplier() { return SpeedUpMultiplier; }

    virtual void ApplyToSnake(AHSSnakeCharacter* SnakeCharacter) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinSpeedUpMultiplier = 1.5f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxSpeedUpMultiplier = 2.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinTimeOfBoost = 5.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxTimeOfBoost = 10.0f;

private:
    virtual void GenerateImprovement(int32 ImprovementLevel) override;

    float TimeOfBoost;
    float SpeedUpMultiplier;
};
