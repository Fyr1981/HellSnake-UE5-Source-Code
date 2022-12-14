// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/HSHUD.h"
#include "HSGameHUD.generated.h"

class AHSSnakeCharacter;
class AHSPlayerController;

UCLASS()
class SNAKEGAME_API AHSGameHUD : public AHSHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintCallable)
    FOnImprovementClick OnImprovementClick;

    UFUNCTION(BlueprintCallable)
    AHSSnakeCharacter* GetSnakeCharacter();

    UFUNCTION(BlueprintCallable)
    AHSPlayerController* GetHSController();

    UFUNCTION(BlueprintCallable)
    void ExitPauseMenu();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> InGameWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> ImprovementsWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> PauseMenuWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> PauseSettingsWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> PauseToMenuWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> GameOverWidgetClass;
    //  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    //  TSubclassOf<UHSUserWidget> PlayersTableWidgetClass;

    UFUNCTION()
    void OnImprovementsGot();
    UFUNCTION()
    void OnImprovementsEnd();
    UFUNCTION()
    void OnPlayerCharacterDestroyed(AActor* DestroyedActor);
    UFUNCTION()
    void OnPauseInput();
    UFUNCTION()
    void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

    void BindSnake();

    UFUNCTION()
    void ChoseImprovement(int32 Index);

    virtual void BeginPlay() override;

    virtual void CreateWidgets() override;
};
