// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSHeadComponent.h"

#include "HSUtils.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/HSTailComponent.h"
#include "Components/HSWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"
#include "Player/HSSnakeCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSSnakeHeadComponent, All, All)

void UHSHeadComponent::BeginPlay()
{
    Super::BeginPlay();

    const auto Snake = Cast<AHSSnakeCharacter>(GetOwner());
    if (!Snake) { return; }
    Snake->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UHSHeadComponent::OnHeadHit);
    Snake->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &UHSHeadComponent::OnHeadOverlap);
}

void UHSHeadComponent::OnHeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    //  UE_LOG(LogSnakeHeadComponent, Display, TEXT("Hit!"));
    Interact(OtherActor, OtherComp);
}

void UHSHeadComponent::OnHeadOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //   UE_LOG(LogSnakeHeadComponent, Display, TEXT("Overlap!"));
    Interact(OtherActor, OtherComp);
}

void UHSHeadComponent::Interact(AActor* OtherActor, UPrimitiveComponent* OtherComp) const
{
    if (GetOwner() == OtherActor)
    {
        //  UE_LOG(LogHSSnakeHeadComponent, Display, TEXT("Interact with owner"));
        if (const auto TailComponent = HSUtils::GetHSPlayerComponent<UHSTailComponent>(GetOwner()))
        {
            if (OtherComp == TailComponent->GetTailCollision(0))
            {
                return;
            }
        }
    }
    if (const auto OtherSnake = Cast<AHSSnakeCharacter>(OtherActor))
    {
        if (OtherComp == OtherSnake->GetHeadComponent())
        {
            return;
        }
    }
    // CanCharacterStepUpOn determines whether an actor can kill a snake
    if (OtherComp->CanCharacterStepUpOn) { return; }
    
    OnHeadDeath.Broadcast();
}
