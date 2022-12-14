// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "HSCoreTypes.h"
#include "HSGameState.generated.h"


UCLASS()
class SNAKEGAME_API AHSGameState : public AGameState
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    TArray<FPlayerData> GetPlayersTable() { return PlayersTable; }

protected:
    TArray<FPlayerData> GetPlayersInfo() const;
    virtual void BeginPlay() override;
private:
    TArray<FPlayerData> PlayersTable;
};
