#pragma once

#include "HSCoreTypes.generated.h"

class UHSBaseImprovement;
class AHSGrenade;
class AHSPlayerController;
class USplineMeshComponent;
class FOnlineSessionSearchResult;

// Improvement

USTRUCT(BlueprintType)
struct FUIImprovementData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    UTexture2D* Icon = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FText Title;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FText Description;
};

// Multiplayer PlayerData

USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString Name;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    int32 Score;
};

inline FName HSMultiplayerSessionName = "HellSnakeSession";

// GameMode

UENUM(BlueprintType)
enum class EClassicModeImprovement : uint8
{
    TemporaryImmortality = 0 UMETA(DisplayName = "Temporary Immortality"),
    GivenGrenade = 1 UMETA(DisplayName = "GivenGrenade"),
    Flexibility = 2 UMETA(DisplayName = "Flexibility")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImprovement, AController*, Controller, const TArray<UHSBaseImprovement*>&, Improvements);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndOfGame);

// HeadComponent

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeadDeath);

// SnakeCharacter

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Controller);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnPlayerController, APlayerController*, PlayerController);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateGrenade, AHSGrenade*, Grenade);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImprovementsEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFire, float, PrepareFireValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImprovementsGot);

// HSHUD

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImprovementClick, int32, ImprovementIndex);

USTRUCT(BlueprintType)
struct FUISessionInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bValid = false;
    UPROPERTY(BlueprintReadWrite)
    FString ServerName;
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentPlayers;
    UPROPERTY(BlueprintReadWrite)
    int32 MaxPlayers;
    UPROPERTY(BlueprintReadWrite)
    int32 Ping;
    
    FOnlineSessionSearchResult* OnlineSessionSearchResult = nullptr;
};

// PlayerState
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncrementScore, AHSPlayerController*, Controller);

// PlayerController
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseInput);

// TailComponent
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnSplineMesh, USplineMeshComponent*, SplineMesh);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddGrowth, int32, Value);
