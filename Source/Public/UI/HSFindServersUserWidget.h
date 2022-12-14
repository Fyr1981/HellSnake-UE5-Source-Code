// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "UI/HSUserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "HSFindServersUserWidget.generated.h"

class FOnlineSessionSearch;
struct FUISessionInfo;
class UButton;
class UEditableText;

UCLASS()
class SNAKEGAME_API UHSFindServersUserWidget : public UHSUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Search")
    int32 MaxSearchResults = 50;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI")
    int32 SessionsOnPage = 5;

    UPROPERTY(meta=(BindWidget))
    UButton* NextPageButton;
    UPROPERTY(meta=(BindWidget))
    UButton* PreviousPageButton;
    UPROPERTY(meta=(BindWidget))
    UEditableText* SearchText;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FUISessionInfo GetUISessionInfo(int32 Index) { return SessionsInfo.IsValidIndex(Index) ? SessionsInfo[Index] : FUISessionInfo(); }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanGoNextPage() { return CurrentPage < GetNumOfPages(); }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanGoPreviousPage() { return CurrentPage > 1; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetNumOfPages();

    UFUNCTION(BlueprintCallable)
    void FindSessions(bool bUseLAN);
    UFUNCTION(BlueprintCallable)
    void JoinSession(int32 IndexOnDisplay);

    UFUNCTION()
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UFUNCTION()
    void OnNextPageButtonClicked();
    UFUNCTION()
    void OnPreviousPageButtonClicked();

private:
    int32 CurrentPage = 1;
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
    TArray<FOnlineSessionSearchResult> SearchResults;
    TArray<FUISessionInfo> SessionsInfo;

    void UpdateSessionsInfo();
};
