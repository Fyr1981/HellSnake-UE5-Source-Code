// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/HSUserWidget.h"
#include "HSCreateServerUserWidget.generated.h"

UCLASS()
class SNAKEGAME_API UHSCreateServerUserWidget : public UHSUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName MultiplayerMapName = "DuelTest";
    
    UFUNCTION(BlueprintCallable)
    void CreateSession(FString ServerName, int32 MaxConnections, bool UseLAN);
    UFUNCTION()
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
};
