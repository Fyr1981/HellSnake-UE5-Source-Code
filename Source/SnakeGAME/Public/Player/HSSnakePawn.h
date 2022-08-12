// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HSCoreTypes.h"
#include "HSPlayerController.h"
#include "HSSnakePawn.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class USoundCue;
class UHSHeadComponent;
class UHSBaseImprovement;
class UHSFlexibilityImprovement;
class UHSTempImmortalityImprovement;
class UHSGivenGrenadeImprovement;
class UPhysicsConstraintComponent;
class USplineComponent;
class UHSSlidingComponent;
class USplineMeshComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class SNAKEGAME_API AHSSnakePawn : public APawn
{
    GENERATED_BODY()

public:
    AHSSnakePawn();

    UPROPERTY(BlueprintAssignable)
    FOnGrowth OnGrowth;
    UPROPERTY(BlueprintAssignable)
    FOnDeath OnDeath;
    UPROPERTY(BlueprintAssignable)
    FOnCreateGrenade OnCreateGrenade;

    UFUNCTION(BlueprintCallable)
    UHSHeadComponent* GetHeadComponent() { return HeadComponent; }

    UFUNCTION(BlueprintCallable)
    UCapsuleComponent* GetHeadCapsule() { return HeadCapsule; }

    UFUNCTION(BlueprintCallable)
    USkeletalMeshComponent* GetTailComponent() { return TailComponent; }

    UFUNCTION(BlueprintCallable)
    USplineComponent* GetSplineComponent() { return SplineComponent; }

    UFUNCTION(BlueprintCallable)
    TArray<USplineMeshComponent*> GetSplineMeshes() { return SplineMeshes; };

    UFUNCTION(BlueprintCallable)
    TArray<UCapsuleComponent*> GetCapsuleComponents() { return CapsuleComponents; };

    UFUNCTION(BlueprintCallable)
    AHSPlayerController* GetSnakeController() { return Cast<AHSPlayerController>(Controller); }

    UFUNCTION(BlueprintCallable)
    float GetDistanceBetweenComponents() { return DistanceBetweenComponents; }

    UFUNCTION(BlueprintCallable)
    UPrimitiveComponent* GetLastComponent();

    UFUNCTION(BlueprintCallable)
    bool IsCanDie() { return bCanDie; };
    UFUNCTION(BlueprintCallable)
    bool IsEating() { return bEating; };

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UHSHeadComponent* HeadComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UCapsuleComponent* HeadCapsule;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* TailComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    USplineComponent* SplineComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UHSSlidingComponent* SlidingComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UStaticMesh* BodyMesh;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (UIMin = "0.0", UIMax = "500.0"))
    float DistanceBetweenComponents = 200.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    FName TailCollisionProfileName = "SnakeTailDefault";
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    FVector BodyMeshScale = FVector(1.0, 1.0, 1.0);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortal")
    UMaterialInterface* ImmortalLeatherMaterial;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortal")
    int32 HeadLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortal")
    int32 TailLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortal")
    int32 BodyLeatherMaterialIndex = 0;

    // Snake should can expansion at all, but you can off this
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Growth")
    bool bCanExpansion = true;
    // Minimum of time between snake expansions
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Growth", meta = (UIMin = "0.0", UIMax = "5.0"))
    float ExpansionRate = 0.5f;

    // Snake can be immortal with setting this false in blueprint
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Health")
    bool bCanDie = true;

    // Start growthing or continue in loop
    UFUNCTION(BlueprintCallable)
    void ContinueGrowth();
    UFUNCTION(BlueprintCallable)
    USplineMeshComponent* SpawnSplineComponent();

    UFUNCTION(BlueprintCallable)
    void Death();

    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

private:
    UPROPERTY()
    TArray<USplineMeshComponent*> SplineMeshes;
    UPROPERTY()
    TArray<UCapsuleComponent*> CapsuleComponents;

    // Timer handle to snake's growth
    FTimerHandle GrowthTimerHandle;
    FTimerHandle ImmortalityTimerHandle;

    // Real number of snake meshes
    int CurrentLength = 0;
    // Number of snake meshes that we want to
    int NeedLength = 0;

    bool bEating = false;

    UFUNCTION()
    void ApplyImprovement(UHSBaseImprovement* Improvement);

    UFUNCTION()
    void Growth();

    void TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost);

    AHSGrenade* CreateGrenade(TSubclassOf<AHSGrenade> GrenadeClass) const;

    void SetMortal();
};
