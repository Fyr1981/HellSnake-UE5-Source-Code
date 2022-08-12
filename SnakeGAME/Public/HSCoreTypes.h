#pragma once

#include "HSCoreTypes.generated.h"


class UHSBaseImprovement;
class AHSGrenade;

// GameMode

UENUM(BlueprintType)
enum class EClassicModeImprovement : uint8
{
    TemporaryImmortality = 0 UMETA(DisplayName = "Temporary Immortality"),
    GivenGrenade = 1 UMETA(DisplayName = "GivenGrenade"),
    Flexibility = 2 UMETA(DisplayName = "Flexibility")
};

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImprovement, AController*, Controller, const TArray<UHSBaseImprovement*>&, Improvements);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndOfGame);

// SnakePawn

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrowth, AController*, Controller);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Controller);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateGrenade, AHSGrenade*, Grenade);

// HeadComponent

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeedUp, float, SpeedUpMultiplier, float, SpeedUpTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEatingApple);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeadDeath);

// PlayerController
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImprovementChoice, UHSBaseImprovement*, Improvement);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFire, float, PrepareFireValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImprovementsGot);

//PickUp

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUp);

// ClassicGameModeUI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImprovementClick, int32, ImprovementIndex);
