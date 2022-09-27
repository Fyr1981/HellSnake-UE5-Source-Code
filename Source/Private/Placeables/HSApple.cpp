// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Placeables/HSApple.h"
#include "HSUtils.h"
#include "Components/HSTailComponent.h"
#include "Player/HSSnakeCharacter.h"

void AHSApple::ApplyToActor(AActor* Actor)
{
    const auto TailComponent = HSUtils::GetHSPlayerComponent<UHSTailComponent>(Actor);
    if (!TailComponent) { return; }
    TailComponent->AddLength();
    Super::ApplyToActor(Actor);
}
