// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSTempImmortalityImprovement.h"
#include "Player/HSSnakeCharacter.h"

void UHSTempImmortalityImprovement::ApplyToSnake(AHSSnakeCharacter* SnakeCharacter)
{
    Super::ApplyToSnake(SnakeCharacter);
    SnakeCharacter->TemporaryImmortality(GetSpeedUpMultiplier(), GetTimeOfBoost());
}

void UHSTempImmortalityImprovement::GenerateImprovement(int32 ImprovementLevel)
{
    SpeedUpMultiplier = FMath::FRandRange(MinSpeedUpMultiplier, MaxSpeedUpMultiplier);
    TimeOfBoost = FMath::FRandRange(MinTimeOfBoost, MaxTimeOfBoost);
    Super::GenerateImprovement(ImprovementLevel);
}
