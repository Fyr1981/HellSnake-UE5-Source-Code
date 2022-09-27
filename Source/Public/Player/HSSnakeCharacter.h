// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HSCoreTypes.h"
#include "GameFramework/Character.h"
#include "HSSnakeCharacter.generated.h"

class UHSMovementComponent;
class UHSHeadComponent;
class UHSTailComponent;
class UCameraComponent;
class AHSPlayerController;
class UHSWeaponComponent;

UCLASS()
class SNAKEGAME_API AHSSnakeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AHSSnakeCharacter(const FObjectInitializer& ObjInit);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(BlueprintAssignable, Category = "Improvements")
    FOnImprovementsEnd OnImprovementsEnd;
    UPROPERTY(BlueprintAssignable, Category = "Improvements")
    FOnImprovementsGot OnImprovementsGot;

    // Getters for components and other actors
    UFUNCTION(BlueprintCallable, Category = "Components")
    UHSHeadComponent* GetHeadComponent();

    UFUNCTION(BlueprintCallable, Category = "Components")
    USkeletalMeshComponent* GetTailTipComponent();

    UFUNCTION(BlueprintCallable, Category = "Components")
    UHSTailComponent* GetTailComponent() { return TailComponent; }

    UFUNCTION(BlueprintCallable, Category = "Components")
    UHSWeaponComponent* GetWeaponComponent() { return WeaponComponent; }

    UFUNCTION(BlueprintCallable, Category = "Components")
    UHSMovementComponent* GetHSMovementComponent();

    UFUNCTION(BlueprintCallable)
    AHSPlayerController* GetSnakeController();

    UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsCanDie() { return bCanDie; }

    UFUNCTION(BlueprintCallable, Category = "Improvements")
    bool IsImprovementQueueEmpty() const { return !ImprovementQueue.Num(); }

    UFUNCTION(BlueprintCallable, Category = "Improvements")
    TArray<FUIImprovementData> GetCurrentUIImprovementDatas() const;

    UFUNCTION(BlueprintCallable, Category = "Improvements")
    void AddImprovements(const TArray<UHSBaseImprovement*> Improvements);

    UFUNCTION(BlueprintCallable, Category = "Improvements")
    void TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost);

    UFUNCTION(NetMulticast, Reliable, Category = "Improvements")
    void CreateGrenade(TSubclassOf<AHSGrenade> GrenadeClass);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Improvements")
    void Server_ChooseImprovement(int32 Index);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UHSTailComponent* TailComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* TailTipComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UHSWeaponComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_bCanDie, Category = "Health")
    bool bCanDie = true;

    UFUNCTION(Category = "Health")
    void OnRep_bCanDie();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortality Mode")
    UMaterialInterface* ImmortalLeatherMaterial;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortality Mode")
    int32 HeadLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortality Mode")
    int32 TailLeatherMaterialIndex = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Immortality Mode")
    int32 BodyLeatherMaterialIndex = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Improvements")
    int32 NumOfImprovements = 3;

    UFUNCTION(BlueprintCallable, Category = "Improvements")
    void ChooseFirstImprovement();
    UFUNCTION(BlueprintCallable, Category = "Improvements")
    void ChooseSecondImprovement();
    UFUNCTION(BlueprintCallable, Category = "Improvements")
    void ChooseThirdImprovement();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Death();

    UFUNCTION(BlueprintCallable, Category = "Growth")
    void OnSpawnSplineMesh(USplineMeshComponent* SplineMeshComponent);

    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

private:
    // Time for end immortality
    FTimerHandle ImmortalityTimerHandle;

    void SetImmortalLeather() const;
    void SetMortalLeather() const;
    void SetMortal();

    UFUNCTION()
    void OnPrepareFire();
    void OnFire();

    // not using TQueue to take many items at the same time
    UPROPERTY(ReplicatedUsing=OnRep_ImprovementQueue)
    TArray<UHSBaseImprovement*> ImprovementQueue;
    UFUNCTION()
    void OnRep_ImprovementQueue();
};
