// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "HSHeadComponent.generated.h"

class AHSPickup;
class AHSGrenade;
class AHSApple;
class AHSSpeedUp;

UCLASS()
class SNAKEGAME_API UHSHeadComponent : public USkeletalMeshComponent
{
    GENERATED_BODY()

public:
    FOnSpeedUp OnSpeedUp;
    FOnEatingApple OnEatingApple;
    FOnHeadDeath OnHeadDeath;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    FName WeaponSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float MaxPrepareFireValue = 5;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float MinThrowImpulse = 1000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float MaxThrowImpulse = 10000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float ThrowAngle = 45.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float TimeToGetGrenadeAgain = 0.5f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float ThrowDistance = 20.0f;

    virtual void BeginPlay() override;
private:
    bool bCanGetGrenade = true;
    FTimerHandle GetGrenadeHandle;

    UFUNCTION()
    void SetCanGetGrenade() { bCanGetGrenade = true; }

    UFUNCTION(Server, Reliable)
    void OnHeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION(Server, Reliable)
    void OnHeadOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
    void Interact(AActor* OtherActor, UPrimitiveComponent* OtherComp);
    UFUNCTION(NetMulticast, Reliable)
    void ThrowGrenade(float PrepareFireValue);

    UFUNCTION()
    void RecieveGrenade(AHSGrenade* Grenade);
    UFUNCTION(NetMulticast, Reliable)
    void AttachGrenade(AHSGrenade* Grenade);
    AHSGrenade* GetAttachedGrenade() const;
    UPROPERTY()
    AHSGrenade* AttachedGrenade = nullptr;

    UFUNCTION(Server, Reliable)
    void TryGetGrenade(AHSPickup* Pickup);
    UFUNCTION(Server, Reliable)
    void TryEatSpeedUp(AHSPickup* Pickup);
    UFUNCTION(Server, Reliable)
    void TryEatApple(AHSPickup* Pickup);
    UFUNCTION(Server, Reliable)
    void TryPickUp(AActor* Actor);

    UFUNCTION(Server, Reliable)
    void NotifyThrowGrenade(float PrepareFireValue);
};
