// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "UI/HSFindServersUserWidget.h"
#include "HSCoreTypes.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "Components/EditableText.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSFindServersWidget, All, All)


bool UHSFindServersUserWidget::Initialize()
{
    const bool Init = Super::Initialize();

    if (NextPageButton) { NextPageButton->OnClicked.AddDynamic(this, &UHSFindServersUserWidget::OnNextPageButtonClicked); }
    if (PreviousPageButton) { PreviousPageButton->OnClicked.AddDynamic(this, &UHSFindServersUserWidget::OnPreviousPageButtonClicked); }

    for (int32 i = 0; i < SessionsOnPage; i++)
    {
        SessionsInfo.Add(FUISessionInfo());
    }
    UpdateSessionsInfo();

    return Init;
}

int32 UHSFindServersUserWidget::GetNumOfPages()
{
    return std::ceil(SearchResults.Num() / SessionsOnPage);
}

void UHSFindServersUserWidget::FindSessions(bool bUseLAN)
{
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface)
    {
        UE_LOG(LogHSFindServersWidget, Display, TEXT("Can not find sessions, no SessionInterface"));
        return;
    }
    SessionInterface->OnFindSessionsCompleteDelegates.RemoveAll(this);
    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UHSFindServersUserWidget::OnFindSessionsComplete);

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = bUseLAN;
    SessionSearch->MaxSearchResults = MaxSearchResults;
    // LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer) { return; }
    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef()))
    {
        UE_LOG(LogHSFindServersWidget, Display, TEXT("Can not complete Find Sessions"));
    }
}

void UHSFindServersUserWidget::JoinSession(int32 IndexOnDisplay)
{
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface) { return; }
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UHSFindServersUserWidget::OnJoinSessionCompleted);
    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),
        HSMultiplayerSessionName,
        *GetUISessionInfo(IndexOnDisplay).OnlineSessionSearchResult);
}

void UHSFindServersUserWidget::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogHSFindServersWidget, Display, TEXT("Find Sessions was complete not successful"));
        return;
    }
    UE_LOG(LogHSFindServersWidget, Display, TEXT("Found %i sessions"), SessionSearch->SearchResults.Num());

    // Try to find server with given name
    if (!SearchText->GetText().IsEmpty())
    {
        UE_LOG(LogHSFindServersWidget, Display, TEXT("Try to find server with name"));
        SearchResults.Empty();
        for (const auto SearchResult : SessionSearch->SearchResults)
        {
            FString SearchResultServerName;
            SearchResult.Session.SessionSettings.Get(SETTING_CUSTOM, SearchResultServerName);
            if (SearchResultServerName.Contains(SearchText->GetText().ToString())) { SearchResults.Add(SearchResult); }
        }
    }
    else
    {
        UE_LOG(LogHSFindServersWidget, Display, TEXT("No server name, get all servers"));
        SearchResults = SessionSearch->SearchResults;
    }
    CurrentPage = 1;
    UpdateSessionsInfo();
}

void UHSFindServersUserWidget::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface) { return; }
    switch (Result)
    {
        case EOnJoinSessionCompleteResult::Success:
        {
            UE_LOG(LogHSFindServersWidget, Display, TEXT("Join session success, travel..."));
            FString ConnectString;
            SessionInterface->GetResolvedConnectString(SessionName, ConnectString);
            GetOwningPlayer()->ClientTravel(ConnectString, TRAVEL_Absolute);
        }
        default: return;
    }
}

void UHSFindServersUserWidget::OnNextPageButtonClicked()
{
    if (!CanGoNextPage()) { return; }
    CurrentPage++;
    UpdateSessionsInfo();
}

void UHSFindServersUserWidget::OnPreviousPageButtonClicked()
{
    if (!CanGoPreviousPage()) { return; }
    CurrentPage--;
    UpdateSessionsInfo();
}

void UHSFindServersUserWidget::UpdateSessionsInfo()
{
    const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld()); //TODO: use Steam
    if (!SessionInterface) { return; }
    for (int32 i = 0; i < SessionsOnPage; i++)
    {
        const int32 SearchResultIndex = (CurrentPage - 1) * SessionsOnPage + i;
        SessionsInfo[i].bValid = SearchResults.IsValidIndex(SearchResultIndex) && SearchResults[SearchResultIndex].IsValid();
        if (!SessionsInfo[i].bValid) { continue; }
        SessionsInfo[i].Ping = SearchResults[SearchResultIndex].PingInMs;
        SessionsInfo[i].MaxPlayers = SearchResults[SearchResultIndex].Session.SessionSettings.NumPublicConnections;
        SearchResults[SearchResultIndex].Session.SessionSettings.Get(SETTING_CUSTOM, SessionsInfo[i].ServerName);
        SessionsInfo[i].OnlineSessionSearchResult = &SearchResults[SearchResultIndex];
        SessionsInfo[i].CurrentPlayers = SearchResults[SearchResultIndex].Session.SessionSettings.NumPublicConnections
                                         - SearchResults[SearchResultIndex].Session.NumOpenPublicConnections;
    }
}
