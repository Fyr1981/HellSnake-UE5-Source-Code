// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSFlexibilityImprovement.h"

#include "HSUtils.h"
#include "Components/HSMovementComponent.h"
#include "Player/HSSnakeCharacter.h"

void UHSFlexibilityImprovement::ApplyToSnake(AHSSnakeCharacter* SnakeCharacter)
{
    Super::ApplyToSnake(SnakeCharacter);
    const auto MovementComponent = HSUtils::GetHSPlayerComponent<UHSMovementComponent>(SnakeCharacter);
    if (!MovementComponent) { return; }
    MovementComponent->ScaleDefaultTurningSpeed(GetTurningSpeedMultiplier());
}

void UHSFlexibilityImprovement::GenerateImprovement(int32 ImprovementLevel)
{
    TurningSpeedMultiplier = FMath::FRandRange(MinTurningSpeedMultiplier, MaxTurningSpeedMultiplier);
    Super::GenerateImprovement(ImprovementLevel);
}
