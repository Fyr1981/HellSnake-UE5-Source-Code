// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Player/HSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Engine/ActorChannel.h"
#include "Improvements/HSBaseImprovement.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/HSSnakeCharacter.h"
#include "UI/HSHUD.h"
#include "HSCoreTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSController, All, All)

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
}

void AHSPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    OnNewPawn.Broadcast(InPawn);
}

void AHSPlayerController::InputPause()
{
    OnPauseInput.Broadcast();
}

void AHSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AHSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("Pause", IE_Pressed, this, &AHSPlayerController::InputPause).bExecuteWhenPaused = true;
}
