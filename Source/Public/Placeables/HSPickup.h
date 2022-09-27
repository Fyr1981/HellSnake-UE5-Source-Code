// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSPickup.generated.h"

class USoundCue;
class UStaticMeshComponent;

UCLASS()
class SNAKEGAME_API AHSPickup : public AActor
{
    GENERATED_BODY()

public:
    AHSPickup();

    UFUNCTION(BlueprintCallable, Category = "Sounds")
    USoundCue* GetOnEatingSound() const { return OnPickUpSound; };

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(EditAnywhere, Category = "Sounds")
    USoundCue* OnPickUpSound;

    UFUNCTION()
    virtual void PickedUp();

    UFUNCTION()
    virtual void ApplyToActor(AActor* Actor);


    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);
};
