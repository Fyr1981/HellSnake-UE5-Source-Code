// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "GameRules/HSGameState.h"
#include "EngineUtils.h"
#include "Player/HSPlayerState.h"

TArray<FPlayerData> AHSGameState::GetPlayersInfo() const
{
    TArray<FPlayerData> PlayersInfo;
    for (TActorIterator<AHSPlayerState> PlayerStateItr(GetWorld()); PlayerStateItr; ++PlayerStateItr)
    {
        FPlayerData PlayerData;
        PlayerData.Name = PlayerStateItr->GetPlayerName();
        PlayerData.Score = FMath::Floor(PlayerStateItr->GetScore());
    }
    return PlayersInfo;
}

void AHSGameState::BeginPlay()
{
    Super::BeginPlay();
    PlayersTable = GetPlayersInfo();
    //TODO: sort table and add on player increment score event and up him in table
}
