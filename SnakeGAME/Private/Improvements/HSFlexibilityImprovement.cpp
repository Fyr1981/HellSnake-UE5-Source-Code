// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSFlexibilityImprovement.h"

void UHSFlexibilityImprovement::GenerateImprovement(int32 ImprovementLevel)
{
    TurningSpeedMultiplier = FMath::FRandRange(MinTurningSpeedMultiplier, MaxTurningSpeedMultiplier);
    Super::GenerateImprovement(ImprovementLevel);
}
