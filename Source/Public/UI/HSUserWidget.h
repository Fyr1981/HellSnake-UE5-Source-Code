// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HSUserWidget.generated.h"

class AHSHUD;

UCLASS()
class SNAKEGAME_API UHSUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetWantsBeShown(bool Wants) { bWantsBeShown = Wants; }

    bool WantsBeShown() const { return bWantsBeShown; }

    bool ShouldHideWidget(UUserWidget* Widget) const;
    
    bool ShouldHideWidgetClass(TSubclassOf<UHSUserWidget> WidgetClass) const;

    UFUNCTION(BlueprintCallable)
    AHSHUD* GetHSHUD() const;

protected:
    // Show or hide exceptions
    // If true, will show exceptions and hide all another classes,
    // if false, will show all widgets except specified classes
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widgets priority")
    bool bShowExceptionsOnly = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Widgets priority", meta=(EditCondition = "!WidgetPriorityMode"))
    TSet<TSubclassOf<UHSUserWidget>> ExceptionsList;

private:
    bool bWantsBeShown = false;
};
