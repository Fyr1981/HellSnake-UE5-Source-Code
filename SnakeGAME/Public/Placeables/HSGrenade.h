// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemComponent.h"
#include "Placeables/HSPickup.h"
#include "HSGrenade.generated.h"

class UNiagaraSystem;
class UHSGivenGrenadeImprovement;
class USphereComponent;
class UFieldSystemComponent;
class URadialFalloff;
class URadialVector;

UCLASS()
class SNAKEGAME_API AHSGrenade : public AHSPickup
{
    GENERATED_BODY()
public:
    AHSGrenade();

    UFUNCTION(BlueprintCallable)
    void SetupWithImprovement(UHSGivenGrenadeImprovement* GrenadeInfo);
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void PullPin();
    UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
    void SetupOnThrown();
    UFUNCTION(BlueprintCallable)
    float GetTimeBeforeExplosion();
    UFUNCTION(BlueprintCallable)
    float GetExplosionRadius();

    virtual void PickedUp_Implementation() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMesh* ThrownMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    USphereComponent* SphereComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UFieldSystemComponent* FieldSystemComponent;
    // UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    // URadialFalloff* RadialFalloff;
    // UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    // URadialVector* RadialVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
    float DestructDamage = 500000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
    float ExplosionForce = 100000000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
    UNiagaraSystem* ExplosionFX;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundCue* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
    float TimeBeforeExplosion = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Status")
    bool bSetuppedByImprovement = false;

private:
    FTimerHandle ExplosionTimer;

    UFUNCTION(NetMulticast, Reliable)
    void Explosion();
};
