// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSUserWidget.h"
#include "UI/HSHUD.h"

bool UHSUserWidget::ShouldHideWidget(UUserWidget* Widget) const
{
    return ShouldHideWidgetClass(Widget->GetClass());
}

bool UHSUserWidget::ShouldHideWidgetClass(TSubclassOf<UHSUserWidget> WidgetClass) const
{
    const bool Contains = ExceptionsList.Contains(WidgetClass);
    return (bShowExceptionsOnly || Contains) && !(bShowExceptionsOnly && Contains);
}

AHSHUD* UHSUserWidget::GetHSHUD() const
{
    return Cast<AHSHUD>(GetOwningPlayer()->GetHUD());
}
