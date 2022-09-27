// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Placeables/HSSpeedUp.h"

#include "HSUtils.h"
#include "Components/HSMovementComponent.h"

void AHSSpeedUp::ApplyToActor(AActor* Actor)
{
    const auto SnakeMovementComponent = HSUtils::GetHSPlayerComponent<UHSMovementComponent>(Actor);
    if (!SnakeMovementComponent) { return; }
    SnakeMovementComponent->ChangeSpeedForTime(GetSpeedUpMultiplier(), GetTimeOfBoost());
    Super::ApplyToActor(Actor);
}
