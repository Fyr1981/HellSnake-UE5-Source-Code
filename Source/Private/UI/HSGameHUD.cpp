// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSPlayerController.h"
#include "Player/HSSnakeCharacter.h"
#include "UI/HSUserWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSGameHUD, All, All)

void AHSGameHUD::BeginPlay()
{
    Super::BeginPlay();
    
    check(GetOwningPlayerController());
    BindSnake();
    if (const auto HSController = GetHSController())
    {
        HSController->OnPauseInput.AddDynamic(this, &AHSGameHUD::OnPauseInput);
        HSController->OnPossessedPawnChanged.AddDynamic(this, &AHSGameHUD::OnPossessedPawnChanged);
    }
    OnImprovementClick.AddDynamic(this, &AHSGameHUD::ChoseImprovement);
    
    TryToShowWidget(GetWidgetByClass(InGameWidgetClass));
}

void AHSGameHUD::CreateWidgets()
{
    Super::CreateWidgets();
    Widgets.Add(InGameWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), InGameWidgetClass));
    Widgets.Add(ImprovementsWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), ImprovementsWidgetClass));
    // @TODO:  Widgets.Add(PlayersTableWidgetClass, CreateWidget<UHSUserWidget>(this, PlayersTableWidgetClass));
    Widgets.Add(PauseMenuWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), PauseMenuWidgetClass));
    Widgets.Add(GameOverWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), GameOverWidgetClass));
    Widgets.Add(PauseSettingsWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), PauseSettingsWidgetClass));
    Widgets.Add(PauseToMenuWidgetClass, CreateWidget<UHSUserWidget>(GetWorld(), PauseToMenuWidgetClass));
}

AHSSnakeCharacter* AHSGameHUD::GetSnakeCharacter()
{
    return Cast<AHSSnakeCharacter>(GetOwningPawn());
}

AHSPlayerController* AHSGameHUD::GetHSController()
{
    return Cast<AHSPlayerController>(GetOwningPlayerController());
}

void AHSGameHUD::ExitPauseMenu()
{
    OnPauseInput();
}

void AHSGameHUD::OnImprovementsGot()
{
    TryToShowWidget(GetWidgetByClass(ImprovementsWidgetClass));
    UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AHSGameHUD::OnImprovementsEnd()
{
    HideWidget(GetWidgetByClass(ImprovementsWidgetClass));
    if (!GetWidgetByClass(PauseMenuWidgetClass)->WantsBeShown())
    {
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }
}

void AHSGameHUD::OnPlayerCharacterDestroyed(AActor* DestroyedActor)
{
    TryToShowWidget(GetWidgetByClass(GameOverWidgetClass));
}

void AHSGameHUD::OnPauseInput()
{
    UHSUserWidget* PauseWidget = GetWidgetByClass(PauseMenuWidgetClass);
    if (PauseWidget->WantsBeShown())
    {
        HideWidget(PauseWidget);
        HideWidget(GetWidgetByClass(PauseSettingsWidgetClass));
        HideWidget(GetWidgetByClass(PauseToMenuWidgetClass));
        if (!GetWidgetByClass(ImprovementsWidgetClass)->WantsBeShown())
        {
            UGameplayStatics::SetGamePaused(GetWorld(), false);
        }
    }
    else
    {
        TryToShowWidget(PauseWidget);
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
}

void AHSGameHUD::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
    BindSnake();
}

void AHSGameHUD::BindSnake()
{
    if (const auto SnakeCharacter = GetSnakeCharacter())
    {
        UE_LOG(LogHSGameHUD, Display, TEXT("Bind Snake"))
        SnakeCharacter->OnImprovementsGot.AddDynamic(this, &AHSGameHUD::OnImprovementsGot);
        SnakeCharacter->OnImprovementsEnd.AddDynamic(this, &AHSGameHUD::OnImprovementsEnd);
        SnakeCharacter->OnDestroyed.AddDynamic(this, &AHSGameHUD::OnPlayerCharacterDestroyed);
    }
}

void AHSGameHUD::ChoseImprovement(int32 Index)
{
    if (GetSnakeCharacter()) { GetSnakeCharacter()->Server_ChooseImprovement(Index); }
}
