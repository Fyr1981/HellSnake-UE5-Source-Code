// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HSWeaponComponent.generated.h"

class AHSGrenade;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SNAKEGAME_API UHSWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHSWeaponComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void RecieveGrenade(AHSGrenade* Grenade);

    UFUNCTION(BlueprintCallable)
    bool CanGetGrenade() { return bCanGetGrenade; }

    UFUNCTION(BlueprintCallable)
    AHSGrenade* GetAttachedGrenade() const;

    FName GetWeaponSocketName() const { return WeaponSocketName; }

    UFUNCTION(BlueprintCallable)
    void InputPrepareFire();

    UFUNCTION(BlueprintCallable)
    void InputFire();

    UFUNCTION(BlueprintCallable)
    void SetPlayerMesh(USkeletalMeshComponent* Mesh) { PlayerMesh = Mesh; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    FName WeaponSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Weapon")
    float MaxPrepareFireTime = 10.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float MinThrowImpulse = 1000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float MaxThrowImpulse = 10000.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float ThrowAngle = 45.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float TimeToGetGrenadeAgain = 0.5f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
    float ThrowDistance = 20.0f;

private:
    FTimerHandle GetGrenadeHandle;

    UPROPERTY()
    USkeletalMeshComponent* PlayerMesh = nullptr;

    bool bCanGetGrenade = true;
    UPROPERTY()
    float PrepareFireTime = -1.f;
    UPROPERTY(ReplicatedUsing=OnRep_PrepareFireDuration)
    float PrepareFireDuration = 0.f;

    UFUNCTION()
    void OnRep_PrepareFireDuration();

    UFUNCTION()
    void AttachGrenade(AHSGrenade* Grenade) const;
    UFUNCTION()
    void ThrowGrenade();

    UFUNCTION()
    void SetCanGetGrenade() { bCanGetGrenade = true; }

    UFUNCTION(Server, Reliable)
    void Server_PrepareFire();
    UFUNCTION(Server, Reliable)
    void Server_Fire();
};
