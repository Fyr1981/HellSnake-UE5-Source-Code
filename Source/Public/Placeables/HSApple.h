// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Placeables/HSPickup.h"
#include "HSApple.generated.h"

UCLASS()
class SNAKEGAME_API AHSApple : public AHSPickup
{
    GENERATED_BODY()

protected:
    virtual void ApplyToActor(AActor* Actor) override;
};
