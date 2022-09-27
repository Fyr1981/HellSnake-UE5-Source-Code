// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Player/HSPlayerState.h"

#include "Components/HSTailComponent.h"
#include "Player/HSPlayerController.h"
#include "Player/HSSnakeCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSPlayerState, All, All)

AHSPlayerState::AHSPlayerState()
{
    bReplicates = true;
}

void AHSPlayerState::BeginPlay()
{
    Super::BeginPlay();
    if (!GetPlayerController()) { return; }
    GetPlayerController()->GetOnNewPawnNotifier().AddUObject(this, &AHSPlayerState::OnNewPawn);
    OnNewPawn(GetPlayerController()->GetPawn());
}

void AHSPlayerState::OnNewPawn(APawn* Pawn)
{
    const auto Snake = Cast<AHSSnakeCharacter>(Pawn);
    if (!Snake || Snake->GetTailComponent()->OnAddGrowth.IsBound()) { return; }
    UE_LOG(LogHSPlayerState, Display, TEXT("Bind snake!"));
    Snake->GetTailComponent()->OnAddGrowth.AddDynamic(this, &AHSPlayerState::OnSnakeGrowth);
}

void AHSPlayerState::OnSnakeGrowth(int32 Value)
{
    //     if (!HasAuthority()) { return; }
    SetScore(Value);
    //     OnRep_Score();
    OnIncrementScore.Broadcast(Cast<AHSPlayerController>(GetPlayerController()));
    UE_LOG(LogHSPlayerState, Display, TEXT("Increment score! %f"), GetScore())
}

void AHSPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
    UE_LOG(LogHSPlayerState, Display, TEXT("Score replicated! %f"), GetScore())
}
