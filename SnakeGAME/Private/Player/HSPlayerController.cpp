// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Player/HSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/HSClassicGameMode.h"
#include "Improvements/HSBaseImprovement.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSPlayerState.h"
#include "Player/HSSnakeCharacter.h"
#include "UI/HSClassicGameModeHUD.h"

class AHSPlayerState;
DEFINE_LOG_CATEGORY_STATIC(LogHSController, All, All)

FVector AHSPlayerController::GetDesiredDirection(FVector CurrentLocation)
{
    if (!PlayerState) { return FVector::ZeroVector; }
    if (PlayerState->IsSpectator()) { return FVector::ZeroVector; }

    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
    const FVector MouseLocation(HitResult.Location.X, HitResult.Location.Y, CurrentLocation.Z);
    return (MouseLocation - CurrentLocation).GetSafeNormal();
}

FVector AHSPlayerController::GetLocationUnderCursor()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    return HitResult.Location;
}

void AHSPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetShowMouseCursor(true);

    if (const auto SnakeState = GetPlayerState<AHSPlayerState>())
    {
        SnakeState->OnImprovement.AddDynamic(this, &AHSPlayerController::GetImprovements);
    }

    if (const auto HUD = GetHUD<AHSClassicGameModeHUD>())
    {
        HUD->OnImprovementClick.AddDynamic(this, &AHSPlayerController::ChooseImprovement);
    }
}

void AHSPlayerController::GetImprovements(AController* Controller, const TArray<UHSBaseImprovement*>& Improvements)
{
    if (Controller != Cast<AController>(this)) { return; }
    ImprovementQueue.Add(Improvements);
    OnImprovementsGot.Broadcast();
    SetInputMode(FInputModeGameAndUI());
}

void AHSPlayerController::ChooseFirst()
{
    ChooseImprovement(0);
}

void AHSPlayerController::ChooseSecond()
{
    ChooseImprovement(1);
}

void AHSPlayerController::ChooseThird()
{
    ChooseImprovement(2);
}

void AHSPlayerController::PrepareFire()
{
    GetWorldTimerManager().SetTimer(PrepareFireHandle, this, &AHSPlayerController::Fire, MaxPrepareFireTime);

    UE_LOG(LogHSController, Display, TEXT("Fire Prepare..."));
}

void AHSPlayerController::Fire()
{
    if (GetWorldTimerManager().TimerExists(PrepareFireHandle))
    {
        PrepareFireValue = GetWorldTimerManager().GetTimerElapsed(PrepareFireHandle);
    }
    OnFire.Broadcast(PrepareFireValue);
    UE_LOG(LogHSController, Display, TEXT("Fire! Value = %f"), PrepareFireValue);
    PrepareFireValue = 0.0f;
    GetWorldTimerManager().ClearTimer(PrepareFireHandle);
}

void AHSPlayerController::ChooseImprovement(int32 Index)
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    UE_LOG(LogHSController, Display, TEXT("Try to chose %i improvement"), Index+1);
    if (!ImprovementQueue.Num()) { return; }
    if (!Cast<AHSSnakeCharacter>(GetPawnOrSpectator())) { return; }
    UE_LOG(LogHSController, Display, TEXT("Try to get %i improvement"), Index+1);
    if (Index >= ImprovementQueue[0].Num()) { return; }
    UHSBaseImprovement* ChosenImprovement = ImprovementQueue[0][Index];
    if (!ChosenImprovement) { return; }
    UE_LOG(LogHSController, Display, TEXT("Chosen improvement name: %s"), *ChosenImprovement->GetName());
    OnImprovementChoice.Broadcast(ChosenImprovement);
    // Free Queue
    for (const auto Improvement : ImprovementQueue[0])
    {
        Improvement->ConditionalBeginDestroy();
    }
    ImprovementQueue.RemoveAt(0);
}

TArray<FUIImprovementData> AHSPlayerController::GetCurrentUIImprovementDatas() const
{
    TArray<FUIImprovementData> UIImrovementDatas;
    if (IsImprovementQueueEmpty()) { return UIImrovementDatas; }

    auto ImprovementQueueElement = ImprovementQueue[0];
    if (ImprovementQueueElement.IsEmpty()) { return UIImrovementDatas; }
    for (const auto Improvement : ImprovementQueueElement)
    {
        UIImrovementDatas.Add(Improvement->GetUIData());
    }

    return UIImrovementDatas;
}

void AHSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("First", IE_Pressed, this, &AHSPlayerController::ChooseFirst).bExecuteWhenPaused = true;
    InputComponent->BindAction("Second", IE_Pressed, this, &AHSPlayerController::ChooseSecond).bExecuteWhenPaused = true;
    InputComponent->BindAction("Third", IE_Pressed, this, &AHSPlayerController::ChooseThird).bExecuteWhenPaused = true;
    InputComponent->BindAction("UseWeapon", IE_Pressed, this, &AHSPlayerController::PrepareFire);
    InputComponent->BindAction("UseWeapon", IE_Released, this, &AHSPlayerController::Fire);
}
