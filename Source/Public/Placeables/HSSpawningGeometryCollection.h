// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "HSSpawningGeometryCollection.generated.h"

UCLASS()
class SNAKEGAME_API AHSSpawningGeometryCollection : public AGeometryCollectionActor
{
    GENERATED_BODY()

public:
    AHSSpawningGeometryCollection();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<AActor> SpawningClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn", meta=(UIMin= "0", UIMax = "20"))
    int32 SpawningMaxCount = 1;

    virtual void BeginPlay() override;

private:
    int32 ActorsSpawned = 0;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SpawnActorInside(const FChaosBreakEvent& BreakEvent);
};
