// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Player/HSPlayerState.h"

void AHSPlayerState::IncrementScore()
{
    SetScore(GetScore() + 1);
}
