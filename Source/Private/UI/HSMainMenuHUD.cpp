// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSMainMenuHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HSUserWidget.h"

void AHSMainMenuHUD::StartSingleGame()
{
}

void AHSMainMenuHUD::CreateWidgets()
{
    Super::CreateWidgets();
    Widgets.Add(MainMenuWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), MainMenuWidgetClass));
    Widgets.Add(SettingsWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), SettingsWidgetClass));
    Widgets.Add(MultiplayerMenuWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), MultiplayerMenuWidgetClass));
    Widgets.Add(GamesListWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), GamesListWidgetClass));
    Widgets.Add(CreateMultiplayerGameWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), CreateMultiplayerGameWidgetClass));
}

void AHSMainMenuHUD::BeginPlay()
{
    Super::BeginPlay();
    TryToShowWidget(GetWidgetByClass(MainMenuWidgetClass));
}
