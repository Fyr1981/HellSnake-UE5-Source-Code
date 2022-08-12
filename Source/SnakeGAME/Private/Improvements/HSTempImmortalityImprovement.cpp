// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSTempImmortalityImprovement.h"

DEFINE_LOG_CATEGORY_STATIC(LogTempImprovement, All, All)

void UHSTempImmortalityImprovement::GenerateImprovement(int32 ImprovementLevel)
{
    SpeedUpMultiplier = FMath::FRandRange(MinSpeedUpMultiplier, MaxSpeedUpMultiplier);
    TimeOfBoost = FMath::FRandRange(MinTimeOfBoost, MaxTimeOfBoost);
    Super::GenerateImprovement(ImprovementLevel);
}
