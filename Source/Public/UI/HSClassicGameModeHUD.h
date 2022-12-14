// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "HSCoreTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HSClassicGameModeHUD.generated.h"

UCLASS()
class SNAKEGAME_API AHSClassicGameModeHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintCallable)
    FOnImprovementClick OnImprovementClick;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void ShowImprovementsWBP();
    UFUNCTION(BlueprintImplementableEvent)
    void CloseImprovementsWBP(UHSBaseImprovement* ImprovementChosen);

private:
    virtual void BeginPlay() override;
};
