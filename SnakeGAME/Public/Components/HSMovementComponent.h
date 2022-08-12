// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HSMovementComponent.generated.h"

class USplineComponent;
class AHSSnakeCharacter;
UCLASS()
class SNAKEGAME_API UHSMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UHSMovementComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void Update();

    UFUNCTION(BlueprintCallable)
    float GetCurrentSpeed() const { return MaxWalkSpeed; }

    UFUNCTION(BlueprintCallable)
    AHSSnakeCharacter* GetSnakeOwner() const;

    // Boost of speed
    UFUNCTION(BlueprintCallable)
    void ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost);

    UFUNCTION(BlueprintCallable)
    void ScaleDefaultTurningSpeed(float Multiplier);

    bool IsSetupped() const;
    void SetupPointers();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    bool bCanMove = true;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    bool bCanTurn = true;

    // Reset any boosts
    UFUNCTION(BlueprintCallable)
    void SetupMoveAndTurnSpeedByDefault();

    virtual void BeginPlay() override;

private:
    FTimerHandle SpeedUpTimerHandle;

    UPROPERTY()
    AHSSnakeCharacter* SnakeOwner;
    UPROPERTY()
    USplineComponent* OwnerSplineComponent;

    // [UnrealUnits per second] Speed without any boosts or decreasements
    float DefaultMovementSpeed = MaxWalkSpeed;
    // [Degrees per second] Max value that snake head can be turned on
    float DefaultYawSpeed = RotationRate.Yaw;

    void MoveAll(float DeltaTime);
    void MoveHead(float DeltaTime);
    void UpdateSplinePoints() const;
    void UpdateSplinePoint(int32 Index) const;
    void UpdateSplineMeshes() const;
    void UpdateSplineMesh(int32 Index) const;
    void UpdateBodyCollision(int32 Index) const;
    void UpdateTailMesh() const;
    void StopSpeedUp();
};
