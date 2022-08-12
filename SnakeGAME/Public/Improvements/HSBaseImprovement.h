// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "UObject/NoExportTypes.h"
#include "HSBaseImprovement.generated.h"

UCLASS(Blueprintable)
class SNAKEGAME_API UHSBaseImprovement : public UObject
{
    GENERATED_BODY()

public:
    // return false if Improvement already Initialized
    UFUNCTION(BlueprintCallable)
    bool InitImprovement(int32 ImprovementLevel);

    UFUNCTION(BlueprintCallable)
    FUIImprovementData GetUIData() const { return UIData; }

    UFUNCTION(BlueprintCallable)
    int32 GetLevel() const { return Level; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "UI")
    FUIImprovementData UIData;

    // Initialize Improvement's characteristics randomly
    virtual void GenerateImprovement(int32 ImprovementLevel)
    {
    }

private:
    bool bInitialized = false;
    int32 Level = 1;
};
