// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSBaseImprovement.h"
#include "Net/UnrealNetwork.h"

bool UHSBaseImprovement::InitImprovement(int32 ImprovementLevel)
{
    if (bInitialized) { return false; }
    Level = ImprovementLevel;
    GenerateImprovement(ImprovementLevel);
    bInitialized = true;
    return true;
}

void UHSBaseImprovement::ApplyToSnake(AHSSnakeCharacter* SnakeCharacter)
{
}

void UHSBaseImprovement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UHSBaseImprovement, UIData)
}

void UHSBaseImprovement::GenerateImprovement(int32 ImprovementLevel)
{
}
