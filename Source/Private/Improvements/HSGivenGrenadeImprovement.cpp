// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Player/HSSnakeCharacter.h"

void UHSGivenGrenadeImprovement::ApplyToSnake(AHSSnakeCharacter* SnakeCharacter)
{
    Super::ApplyToSnake(SnakeCharacter);
    SnakeCharacter->CreateGrenade(GetGrenadeClass());
}
