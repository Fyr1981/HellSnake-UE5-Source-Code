// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSCreateServerUserWidget.h"
#include "HSCoreTypes.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSCreateServerWidget, All, All)

void UHSCreateServerUserWidget::CreateSession(FString ServerName, int32 MaxConnections, bool UseLAN)
{
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface)
    {
        UE_LOG(LogHSCreateServerWidget, Display, TEXT("Can not Create session, no SessionInterface"))
        return;
    }

    SessionInterface->OnCreateSessionCompleteDelegates.RemoveAll(this);
    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UHSCreateServerUserWidget::OnCreateSessionComplete);

    auto SessionSettings = FOnlineSessionSettings();
    SessionSettings.NumPublicConnections = MaxConnections;
    SessionSettings.bIsLANMatch = UseLAN;
    SessionSettings.NumPrivateConnections = 0;
    SessionSettings.bAllowInvites = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bShouldAdvertise = true;

    // Use custom setting to create custom server name
    SessionSettings.Set(SETTING_CUSTOM, ServerName, EOnlineDataAdvertisementType::ViaOnlineService);
    //TODO: use to open need map
    SessionSettings.Set(SETTING_MAPNAME, MultiplayerMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), HSMultiplayerSessionName, SessionSettings))
    {
        UE_LOG(LogHSCreateServerWidget, Display, TEXT("Can not complete Create session"));
    }
}

void UHSCreateServerUserWidget::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogHSCreateServerWidget, Display, TEXT("Create session was complete not successful"))
        return;
    }
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface) { return; }
    UE_LOG(LogHSCreateServerWidget, Display, TEXT("Create session complete successful"))
    UGameplayStatics::OpenLevel(GetWorld(), MultiplayerMapName, true, "Listen");
    SessionInterface->StartSession(SessionName);
    //TODO:  start session in game mode when players are ready
}
