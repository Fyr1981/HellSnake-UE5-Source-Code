// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HSCoreTypes.h"
#include "HSGameMode.generated.h"

class AController;
class APlayerController;
class UHSBaseImprovement;

UCLASS()
class SNAKEGAME_API AHSGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AHSGameMode();

    UPROPERTY(BlueprintAssignable)
    FOnEndOfGame OnEndOfGame;
    UPROPERTY(BlueprintAssignable)
    FOnSpawnPlayerController OnSpawnPlayerController;

protected:
    UFUNCTION(BlueprintCallable)
    int GetScore(AController* Controller) const;
    UFUNCTION(BlueprintCallable)
    void OnIncrementScore(AHSPlayerController* Controller);
    UFUNCTION(BlueprintCallable)
    void OnPlayerDeath(AActor* DestroyedCharacter);
    UFUNCTION(BlueprintCallable)
    TArray<UHSBaseImprovement*> GenerateImprovements(int32 ImprovementLevel, APlayerController* OwningPlayer);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements")
    TArray<int32> BonusPoints;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements", meta = (InlineEditConditionToggle))
    int32 NumberOfImprovements = 3;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Improvements")
    TArray<TSubclassOf<UHSBaseImprovement>> ImprovementTypes;

    void SortBonusPoints();
    virtual void BeginPlay() override;

    virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

    UFUNCTION()
    void OnSpawnPlayer(APlayerController* PlayerController);

    virtual bool ReadyToStartMatch_Implementation() override;
    virtual bool ReadyToEndMatch_Implementation() override;
    virtual void GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL) override;
};
