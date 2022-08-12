// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "HSPlayerController.generated.h"

UCLASS()
class SNAKEGAME_API AHSPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FOnFire OnFire;
    UPROPERTY(BlueprintAssignable)
    FOnImprovementChoice OnImprovementChoice;
    UPROPERTY()
    FOnImprovementsGot OnImprovementsGot;

    UFUNCTION(BlueprintCallable)
    FVector GetDesiredDirection(FVector CurrentLocation);
    UFUNCTION(BlueprintCallable)
    float GetPrepareFireValue() const { return PrepareFireValue; }

    UFUNCTION(BlueprintCallable)
    FVector GetLocationUnderCursor();

    UFUNCTION(BlueprintCallable)
    bool IsImprovementQueueEmpty() const { return !ImprovementQueue.Num(); }

    UFUNCTION(BlueprintCallable)
    TArray<FUIImprovementData> GetCurrentUIImprovementDatas() const;

    virtual void SetupInputComponent() override;
protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Weapon")
    float MaxPrepareFireTime = 10.0f;
    UFUNCTION(BlueprintCallable)
    void GetImprovements(AController* Controller, const TArray<UHSBaseImprovement*>& Improvements);
    UFUNCTION(BlueprintCallable)
    void ChooseFirst();
    UFUNCTION(BlueprintCallable)
    void ChooseSecond();
    UFUNCTION(BlueprintCallable)
    void ChooseImprovement(int32 Index);
    UFUNCTION(BlueprintCallable)
    void ChooseThird();
    UFUNCTION(BlueprintCallable)
    void PrepareFire();
    UFUNCTION(BlueprintCallable)
    void Fire();

private:
    TArray<TArray<UHSBaseImprovement*>> ImprovementQueue;
    FTimerHandle PrepareFireHandle;
    float PrepareFireValue = 0.f;
};
