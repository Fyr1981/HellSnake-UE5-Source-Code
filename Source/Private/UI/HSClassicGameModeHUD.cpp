// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSClassicGameModeHUD.h"
#include "Player/HSPlayerController.h"

void AHSClassicGameModeHUD::BeginPlay()
{
    Super::BeginPlay();

    const auto SnakeController = Cast<AHSPlayerController>(GetOwningPlayerController());
    check(SnakeController);
    SnakeController->OnImprovementsGot.AddDynamic(this, &AHSClassicGameModeHUD::ShowImprovementsWBP);
    SnakeController->OnImprovementChoice.AddDynamic(this, &AHSClassicGameModeHUD::CloseImprovementsWBP);
}
