// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSMovementComponent.h"
#include "Math/UnitConversion.h"
#include "Net/UnrealNetwork.h"
#include "Player/HSSnakeCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSMovementComponent, All, All)

UHSMovementComponent::UHSMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    MaxWalkSpeed = 860.0f;
    RotationRate.Yaw = 250.0f;
    GroundFriction = 32.0f;
    bUseControllerDesiredRotation = true;
}

void UHSMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    DefaultYawSpeed = RotationRate.Yaw;
}

void UHSMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHSMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UHSMovementComponent, AdditionalSpeed)
}

float UHSMovementComponent::GetMaxSpeed() const
{
    return Super::GetMaxSpeed() + AdditionalSpeed;
}

void UHSMovementComponent::ScaleDefaultTurningSpeed(float Multiplier)
{
    DefaultYawSpeed *= Multiplier;
    RotationRate.Yaw = DefaultYawSpeed;
    UE_LOG(LogHSMovementComponent, Display, TEXT("Now snake is more flexible!"));
}

void UHSMovementComponent::ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost)
{
    if (SpeedUpMultiplier < 0 || TimeOfBoost < 0) { return; }
    AdditionalSpeed = GetMaxSpeed() * SpeedUpMultiplier - MaxWalkSpeed;
    GetWorld()->GetTimerManager().ClearTimer(SpeedUpTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SpeedUpTimerHandle,
        this,
        &UHSMovementComponent::ResetAdditionalSpeed,
        TimeOfBoost,
        false);
}

void UHSMovementComponent::ResetAdditionalSpeed()
{
    AdditionalSpeed = 0.0f;
    UE_LOG(LogHSMovementComponent, Display, TEXT("SpeedUp has been stopped!"));
}

void UHSMovementComponent::SetupMoveAndTurnSpeedByDefault()
{
    ResetAdditionalSpeed();
    RotationRate.Yaw = DefaultYawSpeed;
}
