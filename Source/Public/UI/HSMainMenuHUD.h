// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/HSHUD.h"
#include "HSMainMenuHUD.generated.h"

UCLASS()
class SNAKEGAME_API AHSMainMenuHUD : public AHSHUD
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void StartSingleGame();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> MainMenuWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> SettingsWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> MultiplayerMenuWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> GamesListWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widget Classes")
    TSubclassOf<UHSUserWidget> CreateMultiplayerGameWidgetClass;

    virtual void CreateWidgets() override;

    virtual void BeginPlay() override;
};
