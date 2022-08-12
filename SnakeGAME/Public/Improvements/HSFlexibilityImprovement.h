// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Improvements/HSBaseImprovement.h"
#include "HSFlexibilityImprovement.generated.h"

UCLASS(Blueprintable)
class SNAKEGAME_API UHSFlexibilityImprovement : public UHSBaseImprovement
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    float GetTurningSpeedMultiplier() const { return TurningSpeedMultiplier; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinTurningSpeedMultiplier = 1.05f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxTurningSpeedMultiplier = 1.2f;

private:
    virtual void GenerateImprovement(int32 ImprovementLevel) override;

    float TurningSpeedMultiplier;
    float AdditionalFlexibility;
};
