// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HSRandomSpawner.generated.h"

class UBoxComponent;

UCLASS()
class SNAKEGAME_API AHSRandomSpawner : public AActor
{
    GENERATED_BODY()

public:
    AHSRandomSpawner();

protected:
    // Defines spawn boundaries, be careful
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UBoxComponent* BoxComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AActor> SpawningActorClass;
    // Spawn by every destroy of spawning actor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    bool bInfiniteSpawns = true;
    // This actor will be destroyed after this count of spawns
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta=(EditCondition = "!bInfiniteSpawns", ClampMin = "1"))
    int32 MaxCountOfSpawns = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning",
        meta=(EditCondition = "MaxCountOfSpawns > 1", ClampMin = "0", ClampMax = "100"))
    int32 ActorsPerSpawn = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta=(ClampMin = "0", ClampMax = "100"))
    int32 ActorsOnFirstSpawn = 1;

    UFUNCTION(BlueprintCallable)
    void SpawnIteration();
    UFUNCTION(BlueprintCallable)
    FVector GetRandomLocation() const;
    UFUNCTION()
    void OnActorDestroyed(AActor* DestroyedActor);

    virtual void BeginPlay() override;

private:
UFUNCTION(Client, Reliable)
    void Client_SpawnActor(FTransform Transform);
    
    int32 SpawnsHappened = 0;
};
