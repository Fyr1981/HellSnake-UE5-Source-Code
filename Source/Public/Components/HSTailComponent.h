// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "Components/SplineComponent.h"
#include "HSTailComponent.generated.h"

class AHSSnakeCharacter;
class USplineMeshComponent;
class UCapsuleComponent;

UCLASS()
class SNAKEGAME_API UHSTailComponent : public USplineComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnAddGrowth OnAddGrowth;
    UPROPERTY(BlueprintAssignable)
    FOnSpawnSplineMesh OnSpawnSplineMesh;

    UHSTailComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Main function to growth, should be only called on the server
    void AddLength(int32 Length = 1);

    // Update all components in arrays
    void UpdateComponent();

    UCapsuleComponent* GetTailCollision(int32 Index);
    
    TArray<USplineMeshComponent*> GetMeshes() { return SplineMeshes; }
    
    UStaticMesh* GetBodyMesh() const { return BodyMesh; }

    // Must have to normal work, root component is by default
    void SetHeadComponent(USceneComponent* NewHeadComponent) { HeadComponent = NewHeadComponent; }
    // Will used as last tail component, use tail of HSSnakeCharacter by default
    void SetTailTipComponent(USceneComponent* NewTailTipComponent) { TailTipComponent = NewTailTipComponent; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Body")
    UStaticMesh* BodyMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenComponents = 200.0f;
    //  // Move tail up or down to math with head mesh
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body")
    float TailZOffset = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenHeadAndSpline = 50.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenHeadAndBody = 50.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Body")
    FName TailCollisionProfileName = "SnakeTailDefault";

    // Snake should can expansion at all, but you can off this manually
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Growth")
    bool bCanExpansion = true;
    // Time between snake expansions
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Growth", meta = (UIMin = "0.0", UIMax = "5.0"))
    float ExpansionRate = 0.5f;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    USceneComponent* HeadComponent;
    UPROPERTY()
    USceneComponent* TailTipComponent;
    // Assotiated with spline points by indexes
    UPROPERTY()
    TArray<USplineMeshComponent*> SplineMeshes;
    UPROPERTY()
    TArray<UCapsuleComponent*> CapsuleComponents;

    // Loop of growth, should not be cleared ever!
    UPROPERTY()
    FTimerHandle GrowthTimerHandle;

    // Real number of tail meshes
    UPROPERTY()
    int CurrentLength = 0;
    // Number of tail meshes that we want to have TODO: possibility to decrease length
    UPROPERTY(ReplicatedUsing = OnRep_NeedLength)
    int NeedLength = 0;
    UFUNCTION()
    void OnRep_NeedLength() const;
    // Spawn new components
    UFUNCTION()
    void Growth();
    void SpawnSplineMeshAndCollision();
    USplineMeshComponent* SpawnSplineMeshNonRegistered();
    UCapsuleComponent* SpawnSplineCollisionNonRegistered(USplineMeshComponent* SplineMesh);

    // for internal use
    UPrimitiveComponent* GetLastMesh();

    // update all of this things every frame
    
    void UpdateSplinePoints();
    void UpdateSplinePoint(int32 Index);
    void UpdateSplineMeshes() const;
    void UpdateSplineMesh(int32 Index) const;
    void UpdateBodyCollision(int32 Index) const;
    void UpdateTailTip() const;
};
