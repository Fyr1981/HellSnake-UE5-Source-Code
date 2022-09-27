// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Placeables/HSPickup.h"
#include "HSGrenade.generated.h"

class UNiagaraSystem;
class UHSGivenGrenadeImprovement;
class UFieldSystemComponent;
class URadialForceComponent;

UCLASS()
class SNAKEGAME_API AHSGrenade : public AHSPickup
{
    GENERATED_BODY()
public:
    AHSGrenade();
    
    UFUNCTION(BlueprintCallable)
    void PullPin();
    UFUNCTION(BlueprintCallable)
    void SetupOnThrown();
    UFUNCTION(BlueprintCallable)
    float GetTimeBeforeExplosion();
    UFUNCTION(BlueprintCallable)
    float GetExplosionRadius();

    virtual void ApplyToActor(AActor* Actor) override;
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMesh* ThrownMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UFieldSystemComponent* FieldSystemComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    URadialForceComponent* RadialForceComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeometryCollection affect params")
    float DestructDamage = 500000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeometryCollection affect params")
    float FieldSystemStrength = 5000000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
    UNiagaraSystem* ExplosionFX;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundCue* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parameters")
    float TimeBeforeExplosion = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Status")
    bool bSetuppedByImprovement = false;

    virtual void PickedUp() override;

private:
    FTimerHandle ExplosionTimer;

    UFUNCTION()
    void Explosion();
};
