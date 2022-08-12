// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSHeadComponent.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"
#include "Placeables/HSApple.h"
#include "Placeables/HSGrenade.h"
#include "Placeables/HSSpeedUp.h"
#include "Player/HSPlayerController.h"
#include "Player/HSSnakeCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogSnakeHeadComponent, All, All)

void UHSHeadComponent::BeginPlay()
{
    Super::BeginPlay();

    const auto Snake = Cast<AHSSnakeCharacter>(GetOwner());
    // check(Snake);
    const auto Controller = Cast<AHSPlayerController>(Snake->GetController());
    // check(Controller);
    if (!Snake || !Controller) { return; }
    SetNotifyRigidBodyCollision(true);
    Snake->GetCapsuleComponent()->OnComponentHit.AddUniqueDynamic(this, &UHSHeadComponent::OnHeadHit);
    Snake->GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &UHSHeadComponent::OnHeadOverlap);
    Snake->OnCreateGrenade.AddUniqueDynamic(this, &UHSHeadComponent::RecieveGrenade);
    Controller->OnFire.AddUniqueDynamic(this, &UHSHeadComponent::NotifyThrowGrenade);
}

void UHSHeadComponent::OnHeadHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Interact(OtherActor, OtherComp);
}

void UHSHeadComponent::TryEatSpeedUp_Implementation(AHSPickup* Pickup)
{
    //    UE_LOG(LogSnakeHeadComponent, Display, TEXT("Overlapped actor is a SpeedUp!"));
    const auto SpeedUp = Cast<AHSSpeedUp>(Pickup);
    if (!SpeedUp) { return; }
    const float SpeedUpMultiplier = SpeedUp->GetSpeedUpMultiplier();
    const float Time = SpeedUp->GetTimeOfBoost();
    Pickup->PickedUp();
    OnSpeedUp.Broadcast(SpeedUpMultiplier, Time);
}

void UHSHeadComponent::TryEatApple_Implementation(AHSPickup* Pickup)
{
    if (!Cast<AHSApple>(Pickup)) { return; }
    UE_LOG(LogSnakeHeadComponent, Display, TEXT("Eat apple!"));
    Pickup->PickedUp();
    OnEatingApple.Broadcast();
}

void UHSHeadComponent::TryGetGrenade_Implementation(AHSPickup* Pickup)
{
    //  UE_LOG(LogSnakeHeadComponent, Display, TEXT("Overlapped actor is Grenade!"))
    const auto Grenade = Cast<AHSGrenade>(Pickup);
    if (!Grenade) { return; }
    if (GetAttachedGrenade() || !bCanGetGrenade) { return; }

    Grenade->PickedUp();
    AttachGrenade(Grenade);

    AttachedGrenade = GetAttachedGrenade();
    if (!AttachedGrenade) { return; }
    if (AttachedGrenade->GetTimeBeforeExplosion() >= 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(GetGrenadeHandle, this, &UHSHeadComponent::SetCanGetGrenade,
            AttachedGrenade->GetTimeBeforeExplosion());
    }

    bCanGetGrenade = false;
}

void UHSHeadComponent::TryPickUp_Implementation(AActor* Actor)
{
    const auto Pickup = Cast<AHSPickup>(Actor);
    TryEatApple(Pickup);
    TryGetGrenade(Pickup);
    TryEatSpeedUp(Pickup);
}

void UHSHeadComponent::Interact(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
    const auto SnakeOwner = Cast<AHSSnakeCharacter>(GetOwner());
    if (!OtherActor || !OtherComp || !SnakeOwner) { return; }
    //    UE_LOG(LogSnakeHeadComponent, Display, TEXT("Other actor name: %s"), *OtherActor->GetName());
    //    UE_LOG(LogSnakeHeadComponent, Display, TEXT("Other component name: %s"), *OtherComp->GetName());
    if (const auto PickUp = Cast<AHSPickup>(OtherActor))
    {
        TryPickUp(PickUp);
        return;
    }
    if (SnakeOwner->GetCapsuleComponents().Num())
    {
        if (OtherComp == SnakeOwner->GetCapsuleComponents()[0]) { return; }
    }
    if (OtherComp->CanCharacterStepUp(SnakeOwner)) { return; }
    if (SnakeOwner->IsCanDie()) { OnHeadDeath.Broadcast(); }
}

void UHSHeadComponent::ThrowGrenade_Implementation(float PrepareFireValue)
{
    UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Try to find grenade!"))
    AHSGrenade* Grenade = GetAttachedGrenade();
    if (!Grenade) { return; }
    Grenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Should be detached!"))

    //Calculate impulse
    const float ImpulseValue = FMath::Clamp(PrepareFireValue, 0.0f, MaxPrepareFireValue);
    const float Scale = FMath::GetMappedRangeValueClamped(TRange<float>(0.0f, MaxPrepareFireValue), //
        TRange<float>(MinThrowImpulse, MaxThrowImpulse),                                            //
        ImpulseValue);
    const FVector ThrowImpulseOneVector(GetForwardVector().X * FMath::Cos(ThrowAngle), //
        GetForwardVector().Y,                                                          //
        GetForwardVector().Z * FMath::Sin(ThrowAngle));

    if (const auto GrenadeMesh = Cast<UPrimitiveComponent>(Grenade->GetRootComponent()))
    {
        Grenade->PullPin();
        if (GrenadeMesh->GetCollisionEnabled() == ECollisionEnabled::PhysicsOnly ||
            GrenadeMesh->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics)
        {
            //  UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Throw! Scale: %f"), Scale);
            Grenade->AddActorWorldOffset(GetForwardVector() * ThrowDistance);
            GrenadeMesh->AddImpulse(ThrowImpulseOneVector * Scale, FName(), true);
            GetWorld()->GetTimerManager().SetTimer(GetGrenadeHandle, this, &UHSHeadComponent::SetCanGetGrenade, TimeToGetGrenadeAgain);
        }
    }
}

void UHSHeadComponent::RecieveGrenade(AHSGrenade* Grenade)
{
    TryGetGrenade(Grenade);
}

void UHSHeadComponent::AttachGrenade_Implementation(AHSGrenade* Grenade)
{
    const FAttachmentTransformRules GrenadeAttachmentRules(EAttachmentRule::SnapToTarget, //
        EAttachmentRule::SnapToTarget,                                                    //
        EAttachmentRule::SnapToTarget,                                                    //
        true);
    Grenade->PickedUp();
    Grenade->AttachToComponent(this, GrenadeAttachmentRules, WeaponSocketName);
}

AHSGrenade* UHSHeadComponent::GetAttachedGrenade() const
{
    TArray<AActor*> AttachedActors;
    GetOwner()->GetAttachedActors(AttachedActors);
    for (const auto AttachedActor : AttachedActors)
    {
        //   UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Have attached child: %s"), *AttachedActor->GetName())
        if (const auto Grenade = Cast<AHSGrenade>(AttachedActor))
        {
            return Grenade;
            // UE_LOG(LogSnakeHeadComponent, Warning, TEXT("Have grenade!"));
        }
    }
    return nullptr;
}

void UHSHeadComponent::NotifyThrowGrenade_Implementation(float PrepareFireValue)
{
    ThrowGrenade(PrepareFireValue);
}

void UHSHeadComponent::OnHeadOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //  UE_LOG(LogSnakeHeadComponent, Display, TEXT("Head overlap!"));
    Interact(OtherActor, OtherComp);
}
