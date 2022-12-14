// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HSMovementComponent.generated.h"

UCLASS()
class SNAKEGAME_API UHSMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UHSMovementComponent();
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Override main speed function to add additional replicated speed
    virtual float GetMaxSpeed() const override;

    // Boost of speed TODO: add it forever various
    UFUNCTION(BlueprintCallable)
    void ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost);

    // Boost of  turning speed forever
    UFUNCTION(BlueprintCallable)
    void ScaleDefaultTurningSpeed(float Multiplier);

protected:
    virtual void BeginPlay() override;

    // Reset any boosts TODO: add it for time various
    UFUNCTION(BlueprintCallable)
    void SetupMoveAndTurnSpeedByDefault();
    
    UFUNCTION(BlueprintCallable)
    void ResetAdditionalSpeed();

private:
    FTimerHandle SpeedUpTimerHandle;

    UPROPERTY(Replicated)
    float AdditionalSpeed = 0.0f;
    
    float DefaultYawSpeed = RotationRate.Yaw;
};
