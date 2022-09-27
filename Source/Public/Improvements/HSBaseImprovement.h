// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "HSBaseImprovement.generated.h"

class AHSSnakeCharacter;

UCLASS(Blueprintable)
class SNAKEGAME_API UHSBaseImprovement : public UObject
{
    GENERATED_BODY()

public:
    // return false if Improvement already Initialized
    UFUNCTION(BlueprintCallable)
    bool InitImprovement(int32 ImprovementLevel);

    UFUNCTION(BlueprintCallable)
    virtual void ApplyToSnake(AHSSnakeCharacter* SnakeCharacter);

    UFUNCTION(BlueprintCallable)
    FUIImprovementData GetUIData() const { return UIData; }

    UFUNCTION(BlueprintCallable)
    int32 GetLevel() const { return Level; }

    // Replicate improvement
    virtual bool IsSupportedForNetworking() const override { return true; }
    // Replicate only UI data
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "UI", Replicated)
    FUIImprovementData UIData;

    // Initialize Improvement's characteristics randomly, overrides in inheriters
    virtual void GenerateImprovement(int32 ImprovementLevel);

private:
    bool bInitialized = false;
    int32 Level = 1;
};
