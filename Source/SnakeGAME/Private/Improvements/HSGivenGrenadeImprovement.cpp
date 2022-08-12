// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSGivenGrenadeImprovement.h"

void UHSGivenGrenadeImprovement::GenerateImprovement(int32 ImprovementLevel)
{
    ExplosionForce = FMath::FRandRange(MinExplosionForce, MaxExplosionForce);
    ExplosionRadius = FMath::FRandRange(MinExplosionRadius, MaxExplosionRadius);
    TimeBeforeExplosion = FMath::FRandRange(MinTimeBeforeExplosion, MaxTimeBeforeExplosion);
    Super::GenerateImprovement(ImprovementLevel);
}
