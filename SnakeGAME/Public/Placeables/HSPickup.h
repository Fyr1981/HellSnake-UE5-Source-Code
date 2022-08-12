// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "Sound/SoundCue.h"
#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/Actor.h"
#include "HSPickup.generated.h"
class StaticMeshComponent;

UCLASS()
class SNAKEGAME_API AHSPickup : public AActor
{
    GENERATED_BODY()

public:
    AHSPickup();

    UFUNCTION(NetMulticast, Reliable)
    virtual void PickedUp();
    virtual void PickedUp_Implementation();

    UFUNCTION(BlueprintCallable, Category = "Sounds")
    USoundCue* GetOnEatingSound() const { return OnEatingSound; };

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundCue* OnEatingSound;

    virtual void BeginPlay() override;
};
