// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "GameModes/HSClassicGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Improvements/HSBaseImprovement.h"
#include "Player/HSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSPlayerState.h"
#include "Player/HSSnakeCharacter.h"
#include "UI/HSClassicGameModeHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogClassicGameMode, All, All)

AHSClassicGameMode::AHSClassicGameMode()
{
    DefaultPawnClass = AHSSnakeCharacter::StaticClass();
    PlayerControllerClass = AHSPlayerController::StaticClass();
    HUDClass = AHSClassicGameModeHUD::StaticClass();
    PlayerStateClass = AHSPlayerState::StaticClass();
}

void AHSClassicGameMode::BeginPlay()
{
    Super::BeginPlay();
    SortBonusPoints();
    for (TPlayerControllerIterator<AHSPlayerController>::ServerAll It(GetWorld()); It; ++It)
    {
        if (const auto SnakeCharacter = Cast<AHSSnakeCharacter>(It->GetCharacter()))
        {
            SnakeCharacter->OnGrowth.AddDynamic(this, &AHSClassicGameMode::OnAppleEating);
            SnakeCharacter->OnDeath.AddDynamic(this, &AHSClassicGameMode::OnPlayerDeath);
            SnakeCharacter->GetSnakeController()->OnImprovementChoice.AddDynamic(this, &AHSClassicGameMode::OnImprovementChoice);
        }
    }
}

int AHSClassicGameMode::GetScore(AController* Controller) const
{
    if (!Controller) { return 0; }
    return FMath::RoundToInt(Controller->PlayerState->GetScore());
}

void AHSClassicGameMode::SortBonusPoints()
{
    bool Sorted = false;
    int Range = BonusPoints.Num() - 1;
    while (!Sorted)
    {
        Sorted = true;
        for (int i = 0; i < Range; i++)
        {
            if (BonusPoints[i] > BonusPoints[i + 1])
            {
                Swap(BonusPoints[i], BonusPoints[i + 1]);
                Sorted = false;
            }
        }
        Range--;
    }
}

void AHSClassicGameMode::OnAppleEating(AController* Controller)
{
    UE_LOG(LogClassicGameMode, Display, TEXT("On Apple Eating!"));

    const auto SnakeState = Controller->GetPlayerState<AHSPlayerState>();
    if (!SnakeState) { return; }
    SnakeState->IncrementScore();
    const int NewScore = SnakeState->GetScore();
    UE_LOG(LogClassicGameMode, Display, TEXT("Current score: %i"), NewScore);

    if (BonusPoints.Contains(NewScore))
    {
        UE_LOG(LogClassicGameMode, Display, TEXT("Improvement start!"));
        const auto Improvements = GenerateImprovements(BonusPoints.Find(NewScore));
        SnakeState->OnImprovement.Broadcast(Controller, Improvements);
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
}

void AHSClassicGameMode::OnImprovementChoice(UHSBaseImprovement* Improvement)
{
    ResumeGame();
}

void AHSClassicGameMode::ResumeGame()
{
    if (const auto SnakeController = Cast<AHSPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        if (SnakeController->IsImprovementQueueEmpty())
        {
            UGameplayStatics::SetGamePaused(GetWorld(), false);
        }
    }
}

void AHSClassicGameMode::OnPlayerDeath(AController* Controller)
{
    UE_LOG(LogClassicGameMode, Display, TEXT("Game over! Final score: %i"), GetScore(Controller));
    OnEndOfGame.Broadcast();
}

TArray<UHSBaseImprovement*> AHSClassicGameMode::GenerateImprovements(int32 ImprovementLevel)
{
    TArray<UHSBaseImprovement*> NewImprovements;
    if (ImprovementTypes.Num() == 0) { return NewImprovements; }

    auto ImprovementTypesList = ImprovementTypes;
    for (int32 i = 0; i < NumberOfImprovements; ++i)
    {
        const int32 ImprovementTypeIndex = FMath::RandRange(0, ImprovementTypesList.Num() - 1);
        const auto NewImprovementType = ImprovementTypesList[ImprovementTypeIndex];
        UHSBaseImprovement* NewImprovement = NewObject<UHSBaseImprovement>(this, NewImprovementType);
        NewImprovement->InitImprovement(ImprovementLevel);
        NewImprovements.Add(NewImprovement);
        ImprovementTypesList.Remove(NewImprovementType);
    }
    return NewImprovements;
}
