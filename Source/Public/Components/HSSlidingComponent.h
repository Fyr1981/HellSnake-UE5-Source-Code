// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/HSSnakePawn.h"
#include "HSSlidingComponent.generated.h"


class USplineMeshComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SNAKEGAME_API UHSSlidingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHSSlidingComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void Update() const;

    UFUNCTION(BlueprintCallable)
    float GetCurrentSpeed() const { return CurrentMovementSpeed; }

    UFUNCTION(BlueprintCallable)
    AHSSnakePawn* GetSnakeOwner() const { return Cast<AHSSnakePawn>(GetOwner()); }

    // Boost of speed
    UFUNCTION(BlueprintCallable)
    void ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost);

    UFUNCTION(BlueprintCallable)
    void ScaleDefaultTurningSpeed(float Multiplier);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenHeadAndSpline = 50.0f;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    bool bCanMove = true;
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    bool bCanTurn = true;
    // [UnrealUnits per second] Speed without any boosts or decreasements
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (UIMin = "0.0"))
    float DefaultMovementSpeed = 1000;
    // [Degrees per second] Max value that snake head can be turned on
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (UIMin = "0.0"))
    float DefaultTurningSpeed = 360;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (UIMin = "0.0", UIMax = "100.0"))
    float DistantToFloor = 100.0f;

    // Reset any boosts
    UFUNCTION(BlueprintCallable)
    void SetupMoveAndTurnSpeedByDefault();

    virtual void BeginPlay() override;

private:
    FTimerHandle SpeedUpTimerHandle;

    // [UnrealUnits per second] Speed value that can be changed by some things like eating SpeedUp
    float CurrentMovementSpeed;
    // [Degrees per second] Turning speed value that can be changed by some things like eating SpeedUp
    float CurrentTurningSpeed;

    FVector GetDirectionBeforeBlock() const;

    void MoveAll(float DeltaTime) const;
    void MoveHead(float DeltaTime) const;
    void TurnHead(float DeltaTime) const;
    void UpdateSplinePoints() const;
    void UpdateSplinePoint(int32 Index) const;
    void UpdateSplineMeshes() const;
    void UpdateSplineMesh(int32 Index) const;
    void UpdateBodyCollision(int32 Index) const;
    void UpdateTailMesh() const;
    void StopSpeedUp();
};
