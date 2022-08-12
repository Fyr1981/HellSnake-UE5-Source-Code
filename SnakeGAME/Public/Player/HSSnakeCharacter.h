// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/Character.h"
#include "HSSnakeCharacter.generated.h"

class UHSMovementComponent;
class UHSHeadComponent;
class UHSSlidingComponent;
class USplineComponent;
class UCameraComponent;
class AHSPlayerController;
class USplineMeshComponent;
class UCapsuleComponent;

UCLASS()
class SNAKEGAME_API AHSSnakeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AHSSnakeCharacter(const FObjectInitializer& ObjInit);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintAssignable)
    FOnGrowth OnGrowth;
    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;
    UPROPERTY(BlueprintAssignable)
    FOnCreateGrenade OnCreateGrenade;

    // Getters for components and other actors
    UFUNCTION(BlueprintCallable)
    UHSHeadComponent* GetHeadComponent();

    UFUNCTION(BlueprintCallable)
    USkeletalMeshComponent* GetTailComponent();

    UFUNCTION(BlueprintCallable)
    TArray<USplineMeshComponent*> GetSplineMeshes() { return SplineMeshes; }

    UFUNCTION(BlueprintCallable)
    USplineComponent* GetSplineComponent() { return SplineComponent; }

    UFUNCTION(BlueprintCallable)
    TArray<UCapsuleComponent*> GetCapsuleComponents() { return CapsuleComponents; }

    UFUNCTION(BlueprintCallable)
    UCapsuleComponent* GetHeadCapsule() { return GetCapsuleComponent(); }

    UFUNCTION(BlueprintCallable)
    AHSPlayerController* GetSnakeController();

    UFUNCTION(BlueprintCallable)
    float GetDistanceBetweenComponents() { return DistanceBetweenComponents; }

    UFUNCTION(BlueprintCallable)
    float GetDistanceBetweenHeadAndSpline() { return DistanceBetweenHeadAndSpline; }

    UFUNCTION(BlueprintCallable)
    FRotator GetTailRelativeRotation() { return TailRelativeRotation; }

    UFUNCTION(BlueprintCallable)
    float GetTailZOffset() { return TailZOffset; }

    UFUNCTION(BlueprintCallable)
    UPrimitiveComponent* GetLastSplineMeshComponent();

    UFUNCTION(BlueprintCallable)
    UHSMovementComponent* GetHSMovementComponent();

    UFUNCTION(BlueprintCallable)
    bool IsCanDie() { return bCanDie; };

    UFUNCTION(BlueprintCallable)
    bool IsEating() { return bEating; };

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* TailComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    USplineComponent* SplineComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Body")
    UStaticMesh* BodyMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenComponents = 200.0f;
    // Move tail up or down to math with head mesh
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Body")
    float TailZOffset = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Body", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenHeadAndSpline = 50.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Body")
    FName TailCollisionProfileName = "SnakeTailDefault";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Tail")
    FRotator TailRelativeRotation = FRotator(180.0f, 0.0f, 180.0f);

    // Snake should can expansion at all, but you can off this
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components|Growth")
    bool bCanExpansion = true;
    // Minimum of time between snake expansions
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Growth", meta = (UIMin = "0.0", UIMax = "5.0"))
    float ExpansionRate = 0.5f;

    // Snake can be immortal with setting this false in blueprint
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    bool bCanDie = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ImmortalityMode")
    UMaterialInterface* ImmortalLeatherMaterial;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ImmortalityMode")
    int32 HeadLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ImmortalityMode")
    int32 TailLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ImmortalityMode")
    int32 BodyLeatherMaterialIndex = 0;

    UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
    void ContinueGrowth();
    UFUNCTION(BlueprintCallable)
    void SpawnSplineMeshAndCollision();

    UFUNCTION(BlueprintCallable)
    void Death();

    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

private:
    UPROPERTY()
    FTimerHandle GrowthTimerHandle;
    FTimerHandle ImmortalityTimerHandle;

    UPROPERTY()
    TArray<USplineMeshComponent*> SplineMeshes;
    UPROPERTY()
    TArray<UCapsuleComponent*> CapsuleComponents;

    // Real number of snake meshes
    UPROPERTY()
    int CurrentLength = 0;
    // Number of snake meshes that we want to have
    UPROPERTY()
    int NeedLength = 0;
    // May be required by animations
    bool bEating = false;

    // Spawn new components
    UFUNCTION()
    void Growth();
    USplineMeshComponent* SpawnSplineMeshNonRegistered();
    UCapsuleComponent* SpawnSplineCollisionNonRegistered(USplineMeshComponent* SplineMesh);

    // Improvement system
    UFUNCTION()
    void ApplyImprovement(UHSBaseImprovement* Improvement);

    void TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost);
    void SetImmortalLeather();
    void SetMortal();

    AHSGrenade* CreateGrenade(TSubclassOf<AHSGrenade> GrenadeClass) const;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
