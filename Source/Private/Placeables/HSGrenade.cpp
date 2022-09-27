// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Placeables/HSGrenade.h"
#include "HSUtils.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/HSWeaponComponent.h"
#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Kismet/GameplayStatics.h"
#include "Field/FieldSystemComponent.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSGrenade, All, All)

AHSGrenade::AHSGrenade()
{
    FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>("FieldSystemComponent");
    RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("RadialForceComponent");
    RadialForceComponent->SetupAttachment(StaticMeshComponent);
    RadialForceComponent->SetAbsolute(false, false, true);
    RadialForceComponent->ImpulseStrength = 50000.0f;
    RadialForceComponent->ForceStrength = 0.0f;
    RadialForceComponent->Radius = 650.0f;
}

void AHSGrenade::PullPin()
{
    UE_LOG(LogTemp, Display, TEXT("PullPin Authority: %i"), HasAuthority())
    SetupOnThrown();

    if (GetWorldTimerManager().IsTimerActive(ExplosionTimer))
    {
        UE_LOG(LogHSGrenade, Display, TEXT("Tried pull the pin, but timer grenade is already active. TimeBeforeExplosion: %f"),
            GetTimeBeforeExplosion());
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

void AHSGrenade::SetupOnThrown()
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
    return RadialForceComponent->Radius;
}

void AHSGrenade::Explosion()
{
    // Affect phys objects
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation(), GetActorRotation());
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), GetActorRotation());

    // Affect GeometryCollections
    FieldSystemComponent->ApplyStrainField(true, GetActorLocation(), RadialForceComponent->Radius, DestructDamage, 10);
    FieldSystemComponent->ApplyRadialForce(true, GetActorLocation(), FieldSystemStrength);

    RadialForceComponent->FireImpulse();
    UE_LOG(LogHSGrenade, Display, TEXT("Grenade destroyed by explosion! Radius: %f Force: %f"), GetExplosionRadius(),
        RadialForceComponent->ImpulseStrength);
    Destroy();
}

void AHSGrenade::PickedUp()
{
    SetActorEnableCollision(false);
    DisableComponentsSimulatePhysics();
}

void AHSGrenade::ApplyToActor(AActor* Actor)
{
    const auto WeaponComponent = HSUtils::GetHSPlayerComponent<UHSWeaponComponent>(Actor);
    if (!WeaponComponent || !WeaponComponent->CanGetGrenade()) { return; }
    PickedUp();
    WeaponComponent->RecieveGrenade(this);
}
