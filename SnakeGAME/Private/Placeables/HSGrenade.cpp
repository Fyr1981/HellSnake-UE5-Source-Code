// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Placeables/HSGrenade.h"
#include "NiagaraFunctionLibrary.h"
#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSGrenade, All, All)

AHSGrenade::AHSGrenade()
{
    SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComponent->SetupAttachment(StaticMeshComponent);
    SphereComponent->SetAbsolute(false, false, true);
    SphereComponent->SetSphereRadius(650.0f);
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>("FieldSystemComponent");

    //    RadialVector = CreateDefaultSubobject<URadialVector>("RadialVector");
    //    RadialFalloff = CreateDefaultSubobject<URadialFalloff>("RadialFalloff");
}

void AHSGrenade::SetupWithImprovement(UHSGivenGrenadeImprovement* GrenadeInfo)
{
    if (bSetuppedByImprovement || !GrenadeInfo) { return; }
    //  UE_LOG(LogHSGrenade, Display, TEXT("SetupWithImprovement"));
    ExplosionForce = GrenadeInfo->GetExplosionForce();
    SphereComponent->SetSphereRadius(GrenadeInfo->GetExplosionRadius());
    TimeBeforeExplosion = GrenadeInfo->GetTimeBeforeExplosion();
    bSetuppedByImprovement = true;
}

void AHSGrenade::PullPin_Implementation()
{
    SetupOnThrown();

    if (GetWorldTimerManager().IsTimerActive(ExplosionTimer))
    {
        UE_LOG(LogHSGrenade, Display, TEXT("Timer grenade is already active. TimeBeforeExplosion: %f"), GetTimeBeforeExplosion());
        return;
    }

    GetWorldTimerManager().SetTimer(ExplosionTimer, //
        this,                                       //
        &AHSGrenade::Explosion,                     //
        TimeBeforeExplosion,                        //
        false,                                      //
        TimeBeforeExplosion);

    UE_LOG(LogHSGrenade, Display, TEXT("Timer grenade is activate! TimeBeforeExplosion: %f"), GetTimeBeforeExplosion());
}

void AHSGrenade::SetupOnThrown_Implementation()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorEnableCollision(true);
    StaticMeshComponent->SetSimulatePhysics(true);

    StaticMeshComponent->SetStaticMesh(ThrownMesh);
}

float AHSGrenade::GetTimeBeforeExplosion()
{
    return GetWorldTimerManager().GetTimerRemaining(ExplosionTimer);
}

float AHSGrenade::GetExplosionRadius()
{
    return SphereComponent->GetScaledSphereRadius();
}

void AHSGrenade::Explosion_Implementation()
{
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation(), GetActorRotation());
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), GetActorRotation());

    FieldSystemComponent->ApplyStrainField(true, GetActorLocation(), SphereComponent->GetScaledSphereRadius(), DestructDamage, 10);

    FieldSystemComponent->ApplyRadialVectorFalloffForce(true, GetActorLocation(), SphereComponent->GetScaledSphereRadius(), ExplosionForce);

    Destroy();
    UE_LOG(LogHSGrenade, Warning, TEXT("Grenade destroyed by explosion! Radius: %f Force: %f"), GetExplosionRadius(), ExplosionForce);
}

void AHSGrenade::PickedUp_Implementation()
{
    SetActorEnableCollision(false);
    DisableComponentsSimulatePhysics();
}
