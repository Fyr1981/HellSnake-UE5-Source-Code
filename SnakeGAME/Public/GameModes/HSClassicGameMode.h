// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSCoreTypes.h"
#include "HSClassicGameMode.generated.h"

UCLASS()
class SNAKEGAME_API AHSClassicGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AHSClassicGameMode();

    UPROPERTY(BlueprintAssignable)
    FOnEndOfGame OnEndOfGame;

protected:
    UFUNCTION(BlueprintCallable)
    int GetScore(AController* Controller) const;
    UFUNCTION(BlueprintCallable)
    void OnAppleEating(AController* Controller);
    UFUNCTION(BlueprintCallable)
    void OnImprovementChoice(UHSBaseImprovement* Improvement);
    UFUNCTION(BlueprintCallable)
    void ResumeGame();
    UFUNCTION(BlueprintCallable)
    void OnPlayerDeath(AController* Controller);
    UFUNCTION(BlueprintCallable)
    TArray<UHSBaseImprovement*> GenerateImprovements(int32 ImprovementLevel);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements")
    TArray<int32> BonusPoints;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements", meta = (InlineEditConditionToggle))
    int32 NumberOfImprovements = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements | Types")
    TArray<TSubclassOf<UHSBaseImprovement>> ImprovementTypes;

    void SortBonusPoints();
    virtual void BeginPlay() override;
};
