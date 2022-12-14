// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "HSCoreTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HSHUD.generated.h"

class UHSUserWidget;

UCLASS()
class SNAKEGAME_API AHSHUD : public AHUD
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    UHSUserWidget* GetWidgetByClass(TSubclassOf<UHSUserWidget> WidgetClass);

    UFUNCTION(BlueprintCallable)
    bool TryToShowWidget(UHSUserWidget* NewWidget);
    UFUNCTION(BlueprintCallable)
    void HideWidget(UHSUserWidget* WidgetToHide);

protected:
    TMap<TSubclassOf<UHSUserWidget>, UHSUserWidget*> Widgets;

    virtual void BeginPlay() override;

    // Add widgets, should be overrided
    virtual void CreateWidgets();
};
