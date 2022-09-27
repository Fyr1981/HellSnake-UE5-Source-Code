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
    FOnHeadDeath OnHeadDeath;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnHeadOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void Interact(AActor* OtherActor, UPrimitiveComponent* OtherComp) const;
};
