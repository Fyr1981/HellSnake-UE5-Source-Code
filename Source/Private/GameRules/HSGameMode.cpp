// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "GameRules/HSGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Improvements/HSBaseImprovement.h"
#include "Player/HSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSPlayerState.h"
#include "Player/HSSnakeCharacter.h"
#include "UI/HSHUD.h"
#include "GameFramework/GameState.h"
#include "GameRules/HSGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogClassicGameMode, All, All)

AHSGameMode::AHSGameMode()
{
    DefaultPawnClass = AHSSnakeCharacter::StaticClass();
    PlayerControllerClass = AHSPlayerController::StaticClass();
    HUDClass = AHSHUD::StaticClass();
    PlayerStateClass = AHSPlayerState::StaticClass();
    GameStateClass = AHSGameState::StaticClass();

    OnSpawnPlayerController.AddDynamic(this, &AHSGameMode::OnSpawnPlayer);
    //bDelayedStart = true;
}

void AHSGameMode::BeginPlay()
{
    Super::BeginPlay();
    SortBonusPoints();
}

APlayerController* AHSGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    UE_LOG(LogClassicGameMode, Display, TEXT("SpawnPlayerController!"));
    const auto PlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
    OnSpawnPlayerController.Broadcast(PlayerController);
    return PlayerController;
}

//TODO: SetPlayerDefaults
void AHSGameMode::OnSpawnPlayer(APlayerController* PlayerController)
{
    if (const auto SnakeCharacter = Cast<AHSSnakeCharacter>(PlayerController->GetCharacter()))
    {
        SnakeCharacter->OnDestroyed.AddDynamic(this, &AHSGameMode::OnPlayerDeath);
        // SnakeCharacter->OnImprovementsEnd.AddDynamic(this, &AHSGameMode::OnImprovementsEnd);
    }
    if (const auto SnakeState = PlayerController->GetPlayerState<AHSPlayerState>())
    {
        SnakeState->OnIncrementScore.AddDynamic(this, &AHSGameMode::OnIncrementScore);
    }
}

bool AHSGameMode::ReadyToStartMatch_Implementation()
{
    //TODO: Set ready to start in snake controller by input Enter
    return Super::ReadyToStartMatch_Implementation();
}

bool AHSGameMode::ReadyToEndMatch_Implementation()
{
    //TODO: Set by death of all characters
    return Super::ReadyToEndMatch_Implementation();
}

void AHSGameMode::GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL)
{
    //TODO: send message about nubmer of players
    Super::GameWelcomePlayer(Connection, RedirectURL);
}

int AHSGameMode::GetScore(AController* Controller) const
{
    if (!Controller) { return 0; }
    return FMath::RoundToInt(Controller->PlayerState->GetScore());
}

void AHSGameMode::SortBonusPoints()
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

void AHSGameMode::OnIncrementScore(AHSPlayerController* Controller)
{
    if (!Controller) { return; }
    const auto SnakeState = Controller->GetPlayerState<AHSPlayerState>();
    const auto SnakeCharacter = Cast<AHSSnakeCharacter>(Controller->GetCharacter());
    if (!SnakeState || !SnakeCharacter) { return; }
    UE_LOG(LogClassicGameMode, Display, TEXT("Increment Score!"));

    const int NewScore = SnakeState->GetScore();

    if (BonusPoints.Contains(NewScore))
    {
        UE_LOG(LogClassicGameMode, Display, TEXT("Improvement start!"));
        const auto Improvements = GenerateImprovements(BonusPoints.Find(NewScore), Controller);
        SnakeCharacter->AddImprovements(Improvements);
    }
}

void AHSGameMode::OnPlayerDeath(AActor* DestroyedCharacter)
{
    UE_LOG(LogClassicGameMode, Display, TEXT("Game over!"));
    OnEndOfGame.Broadcast();
}

TArray<UHSBaseImprovement*> AHSGameMode::GenerateImprovements(int32 ImprovementLevel, APlayerController* OwningPlayer)
{
    UE_LOG(LogClassicGameMode, Display, TEXT("Try to generate improvements"));
    TArray<UHSBaseImprovement*> NewImprovements;
    if (ImprovementTypes.Num() == 0) { return NewImprovements; }

    auto ImprovementTypesList = ImprovementTypes;
    for (int32 i = 0; i < NumberOfImprovements; ++i)
    {
        const int32 ImprovementTypeIndex = FMath::RandRange(0, ImprovementTypesList.Num() - 1);
        const auto NewImprovementType = ImprovementTypesList[ImprovementTypeIndex];
        auto NewImprovement = NewObject<UHSBaseImprovement>(OwningPlayer, NewImprovementType);
        NewImprovement->InitImprovement(ImprovementLevel);
        NewImprovements.Add(NewImprovement);
        ImprovementTypesList.Remove(NewImprovementType);
    }
    return NewImprovements;
}
