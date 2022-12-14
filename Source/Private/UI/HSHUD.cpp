// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HSUserWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSHUD, All, All)

void AHSHUD::BeginPlay()
{
    Super::BeginPlay();

    CreateWidgets();
    
    for (const auto WidgetPair : Widgets)
    {
        const auto Widget = WidgetPair.Value;
        if (!Widget) { continue; }
        Widget->AddToViewport();
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AHSHUD::CreateWidgets()
{
}

UHSUserWidget* AHSHUD::GetWidgetByClass(TSubclassOf<UHSUserWidget> WidgetClass)
{
    return Widgets.Contains(WidgetClass) ? Widgets[WidgetClass] : nullptr;
}

bool AHSHUD::TryToShowWidget(UHSUserWidget* NewWidget)
{
    if (!NewWidget) { return false; }
    UE_LOG(LogHSHUD, Display, TEXT("Try to show widget: %s"), *NewWidget->GetName())
    NewWidget->SetWantsBeShown(true);
    // if another widget has priority, new widget will be shown only when another will be hidden
    for (const auto WidgetPair : Widgets)
    {
        const auto Widget = WidgetPair.Value;
        if (Widget == NewWidget || !Widget) { continue; }
        if (Widget->ShouldHideWidget(NewWidget) && Widget->Visibility == ESlateVisibility::Visible) { return false; }
    }
    // else hide low priority widgets and show all another
    for (const auto WidgetPair : Widgets)
    {
        const auto Widget = WidgetPair.Value;
        // if it not wants to be shown we dont have any problems with it
        if (Widget == NewWidget || !Widget || !Widget->WantsBeShown()) { continue; }
        // else hide it or show it
        if (NewWidget->ShouldHideWidget(Widget))
        {
            Widget->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            Widget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    // and show itself
    NewWidget->SetVisibility(ESlateVisibility::Visible);
    UE_LOG(LogHSHUD, Display, TEXT("Widget now visible: %s"), *NewWidget->GetName())
    return true;
}

void AHSHUD::HideWidget(UHSUserWidget* WidgetToHide)
{
    if (!WidgetToHide) { return; }
    UE_LOG(LogHSHUD, Display, TEXT("Hide widget: %s"), *WidgetToHide->GetName())
    WidgetToHide->SetWantsBeShown(false);
    WidgetToHide->SetVisibility(ESlateVisibility::Hidden);
    // show all hidden widgets
    for (const auto WidgetPair : Widgets)
    {
        const auto Widget = WidgetPair.Value;
        if (Widget && Widget->WantsBeShown())
        {
            TryToShowWidget(Widget);
        }
    }
}