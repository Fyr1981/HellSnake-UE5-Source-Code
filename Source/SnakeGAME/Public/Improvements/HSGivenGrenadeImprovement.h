// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Improvements/HSBaseImprovement.h"
#include "HSGivenGrenadeImprovement.generated.h"


UCLASS(Blueprintable)
class SNAKEGAME_API UHSGivenGrenadeImprovement : public UHSBaseImprovement
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    float GetExplosionRadius() { return ExplosionRadius; }

    UFUNCTION(BlueprintCallable)
    float GetExplosionForce() { return ExplosionForce; }

    UFUNCTION(BlueprintCallable)
    float GetTimeBeforeExplosion() { return TimeBeforeExplosion; }

    UFUNCTION(BlueprintCallable)
    TSubclassOf<AHSGrenade> GetGrenadeClass() { return GrenadeClass; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<AHSGrenade> GrenadeClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinExplosionForce = 2500.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxExplosionForce = 10000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinExplosionRadius = 250.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxExplosionRadius = 1000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MinTimeBeforeExplosion = 0.5f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Initialize Parameters", meta = (UIMin = "0.0", UIMax = "100.0"))
    float MaxTimeBeforeExplosion = 5.0f;

private:
    virtual void GenerateImprovement(int32 ImprovementLevel) override;

    float ExplosionRadius;
    float ExplosionForce;
    float TimeBeforeExplosion;
};
