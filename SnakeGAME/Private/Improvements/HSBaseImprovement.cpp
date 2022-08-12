// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSBaseImprovement.h"

bool UHSBaseImprovement::InitImprovement(int32 ImprovementLevel)
{
    if (bInitialized) { return false; }
    Level = ImprovementLevel;
    GenerateImprovement(ImprovementLevel);
    bInitialized = true;
    return true;
}
