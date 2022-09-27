// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSWeaponComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Placeables/HSGrenade.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSWeaponComponent, All, All)

UHSWeaponComponent::UHSWeaponComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = false;
    if (const auto Character = Cast<ACharacter>(GetOwner()))
    {
        SetPlayerMesh(Character->GetMesh());
    }
}

void UHSWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UHSWeaponComponent, PrepareFireDuration)
}

void UHSWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    check(PlayerMesh);
}

void UHSWeaponComponent::InputPrepareFire()
{
    UE_LOG(LogHSWeaponComponent, Display, TEXT("Try prepare fire"));
    if (!GetAttachedGrenade()) { return; }
    Server_PrepareFire();
}

void UHSWeaponComponent::Server_PrepareFire_Implementation()
{
    if (!GetAttachedGrenade()) { return; }
    if (!GetWorld()) { return; }
    PrepareFireTime = GetWorld()->GetTimeSeconds();
    UE_LOG(LogHSWeaponComponent, Display, TEXT("Fire Prepare time: %f"), PrepareFireTime);
}


void UHSWeaponComponent::InputFire()
{
    UE_LOG(LogHSWeaponComponent, Display, TEXT("Try fire"));
    if (!GetAttachedGrenade()) { return; }
    Server_Fire();
}

void UHSWeaponComponent::Server_Fire_Implementation()
{
    if (!GetAttachedGrenade()) { return; }
    if (!GetWorld()) { return; }
    UE_LOG(LogHSWeaponComponent, Display, TEXT("Fire Prepare time: %f"), PrepareFireTime);
    if (PrepareFireTime < 0.f) { return; }
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    PrepareFireDuration = CurrentTime - PrepareFireTime;
    OnRep_PrepareFireDuration();
    UE_LOG(LogHSWeaponComponent, Display, TEXT("Fire! Value = %f"), PrepareFireDuration);
    PrepareFireTime = -1.f;
}

void UHSWeaponComponent::ThrowGrenade()
{
    AHSGrenade* Grenade = GetAttachedGrenade();
    if (!Grenade) { return; }
    Grenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    //Calculate impulse
    const float ImpulseValue = FMath::Clamp(PrepareFireDuration, 0.0f, MaxPrepareFireTime);
    const float Scale = FMath::GetMappedRangeValueClamped(TRange<float>(0.0f, MaxPrepareFireTime), //
        TRange<float>(MinThrowImpulse, MaxThrowImpulse),                                           //
        ImpulseValue);
    const FVector ThrowImpulseOneVector(PlayerMesh->GetForwardVector().X * FMath::Cos(ThrowAngle), //
        PlayerMesh->GetForwardVector().Y,                                                          //
        PlayerMesh->GetForwardVector().Z * FMath::Sin(ThrowAngle));

    if (const auto GrenadeMesh = Cast<UPrimitiveComponent>(Grenade->GetRootComponent()))
    {
        Grenade->PullPin();
        if (GrenadeMesh->GetCollisionEnabled() == ECollisionEnabled::PhysicsOnly ||
            GrenadeMesh->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics)
        {
            //  UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Throw! Scale: %f"), Scale);
            Grenade->AddActorWorldOffset(PlayerMesh->GetForwardVector() * ThrowDistance);
            GrenadeMesh->AddImpulse(ThrowImpulseOneVector * Scale, FName(), true);
            GetWorld()->GetTimerManager().SetTimer(GetGrenadeHandle, this, &UHSWeaponComponent::SetCanGetGrenade, TimeToGetGrenadeAgain);
        }
    }
}

void UHSWeaponComponent::RecieveGrenade(AHSGrenade* Grenade)
{
    if (GetAttachedGrenade() || !bCanGetGrenade) { return; }

    AttachGrenade(Grenade);

    const auto AttachedGrenade = GetAttachedGrenade();
    if (!AttachedGrenade) { return; }
    if (AttachedGrenade->GetTimeBeforeExplosion() >= 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(GetGrenadeHandle, this, &UHSWeaponComponent::SetCanGetGrenade,
            AttachedGrenade->GetTimeBeforeExplosion());
    }

    bCanGetGrenade = false;
}

void UHSWeaponComponent::OnRep_PrepareFireDuration()
{
    ThrowGrenade();
}

void UHSWeaponComponent::AttachGrenade(AHSGrenade* Grenade) const
{
    if (!Grenade) { return; }
    const FAttachmentTransformRules GrenadeAttachmentRules(EAttachmentRule::SnapToTarget, //
        EAttachmentRule::SnapToTarget,                                                    //
        EAttachmentRule::SnapToTarget,                                                    //
        true);
    Grenade->AttachToComponent(PlayerMesh, GrenadeAttachmentRules, WeaponSocketName);
}

AHSGrenade* UHSWeaponComponent::GetAttachedGrenade() const
{
    TArray<AActor*> AttachedActors;
    if (!GetOwner()) { return nullptr; }
    GetOwner()->GetAttachedActors(AttachedActors);
    for (const auto AttachedActor : AttachedActors)
    {
        if (const auto Grenade = Cast<AHSGrenade>(AttachedActor))
        {
            return Grenade;
        }
    }
    return nullptr;
}
