// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Player/HSSnakeCharacter.h"

#include "HSUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HSHeadComponent.h"
#include "Components/HSMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Improvements/HSFlexibilityImprovement.h"
#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Improvements/HSTempImmortalityImprovement.h"
#include "Placeables/HSGrenade.h"
#include "Player/HSPlayerController.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSnakeCharacter, All, All)

AHSSnakeCharacter::AHSSnakeCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<UHSMovementComponent>(CharacterMovementComponentName).
                    SetDefaultSubobjectClass<UHSHeadComponent>(MeshComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleHalfHeight(80.0f);
    GetCapsuleComponent()->SetCapsuleRadius(60.0f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SnakeHeadDefault"));
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
    SetRootComponent(GetCapsuleComponent());

    GetMesh()->SetupAttachment(GetCapsuleComponent());

    TailComponent = CreateDefaultSubobject<USkeletalMeshComponent>("TailComponent");
    TailComponent->SetRelativeLocation(FVector(-DistanceBetweenComponents, 0.0, 0.0));
    TailComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TailComponent->SetAbsolute(true, true, true);

    SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
    SplineComponent->SetAbsolute(true, true, true);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(GetRootComponent());
    // Look down
    CameraComponent->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));
    // From a height
    CameraComponent->SetRelativeLocation(FVector(0.0, 0.0, 1500));
    CameraComponent->SetAbsolute(false, true, true);

    BodyMesh = nullptr;
}

void AHSSnakeCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (GetHSMovementComponent()) { GetHSMovementComponent()->Update(); }
}

void AHSSnakeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHSSnakeCharacter::BeginPlay()
{
    Super::BeginPlay();

    //   check(Cast<AHSPlayerController>(GetController()));
    check(GetMesh());
    check(BodyMesh);
    check(CameraComponent);

    GetHeadComponent()->OnHeadDeath.AddDynamic(this, &AHSSnakeCharacter::Death);
    GetHeadComponent()->OnEatingApple.AddDynamic(this, &AHSSnakeCharacter::ContinueGrowth);

    if (bCanExpansion)
    {
        UE_LOG(LogSnakeCharacter, Display, TEXT("Timer growth!"));
        GetWorld()->GetTimerManager().SetTimer(GrowthTimerHandle, this, &AHSSnakeCharacter::Growth, ExpansionRate, true);
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
    if (const auto SnakeController = GetSnakeController())
    {
        SnakeController->OnImprovementChoice.AddDynamic(this, &AHSSnakeCharacter::ApplyImprovement);
    }
    if (!bCanDie) { SetImmortalLeather(); }
}

void AHSSnakeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHSSnakeCharacter::ContinueGrowth_Implementation()
{
    UE_LOG(LogSnakeCharacter, Display, TEXT("Try continue growth!"));
    // TODO: Replicate this variables instead of RPC this function
    if (!bCanExpansion) { return; }
    NeedLength += 1;
    GetWorldTimerManager().UnPauseTimer(GrowthTimerHandle);
}

void AHSSnakeCharacter::Growth()
{
    if (!bCanExpansion) { return; }

    bEating = true;

    const FVector NewLocation = GetLastSplineMeshComponent()->GetComponentLocation()
                                - GetLastSplineMeshComponent()->GetForwardVector() * DistanceBetweenComponents;
    SplineComponent->AddSplineWorldPoint(NewLocation);
    SpawnSplineMeshAndCollision();
    CurrentLength += 1;
    UE_LOG(LogSnakeCharacter, Display, TEXT("Growthed!"));

    if (CurrentLength >= NeedLength)
    {
        bEating = false;
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
    OnGrowth.Broadcast(GetController());
}

void AHSSnakeCharacter::SpawnSplineMeshAndCollision()
{
    UE_LOG(LogSnakeCharacter, Display, TEXT("SpawnSplineComponent!"));
    const auto SplineMesh = SpawnSplineMeshNonRegistered();
    const auto CapsuleCollision = SpawnSplineCollisionNonRegistered(SplineMesh);

    GetHSMovementComponent()->Update();
    SplineMesh->RegisterComponent();
    CapsuleCollision->RegisterComponent();
}

USplineMeshComponent* AHSSnakeCharacter::SpawnSplineMeshNonRegistered()
{
    const auto SplineMesh = NewObject<USplineMeshComponent>(this);
    SplineMesh->SetMobility(EComponentMobility::Movable);
    SplineMesh->SetStaticMesh(BodyMesh);
    SplineMesh->SetAbsolute(true, true, true);
    SplineMesh->SetForwardAxis(ESplineMeshAxis::X);
    SplineMesh->bAlwaysCreatePhysicsState = false;
    if (!bCanDie) { SplineMesh->SetMaterial(BodyLeatherMaterialIndex, ImmortalLeatherMaterial); }
    SplineMeshes.Emplace(SplineMesh);
    return SplineMesh;
}

UCapsuleComponent* AHSSnakeCharacter::SpawnSplineCollisionNonRegistered(USplineMeshComponent* SplineMesh)
{
    const auto CapsuleCollision = NewObject<UCapsuleComponent>(this);
    CapsuleCollision->SetCapsuleSize(SplineMesh->GetStaticMesh()->GetBounds().GetBox().Max.Z,
        SplineMesh->GetStaticMesh()->GetBounds().GetBox().Max.X * 2);
    CapsuleCollision->SetCollisionProfileName(TailCollisionProfileName);
    CapsuleCollision->SetGenerateOverlapEvents(true);
    CapsuleCollision->CanCharacterStepUpOn = ECB_No;
    CapsuleComponents.Emplace(CapsuleCollision);
    return CapsuleCollision;
}

void AHSSnakeCharacter::ApplyImprovement(UHSBaseImprovement* Improvement)
{
    if (const auto TempImmortality = Cast<UHSTempImmortalityImprovement>(Improvement))
    {
        TemporaryImmortality(TempImmortality->GetSpeedUpMultiplier(), TempImmortality->GetTimeOfBoost());
    }
    if (const auto GivenGrenade = Cast<UHSGivenGrenadeImprovement>(Improvement))
    {
        if (!GivenGrenade->GetGrenadeClass()) { return; }
        const auto Grenade = CreateGrenade(GivenGrenade->GetGrenadeClass());
        Grenade->SetupWithImprovement(GivenGrenade);
    }
    if (const auto Flexibility = Cast<UHSFlexibilityImprovement>(Improvement))
    {
        Cast<UHSMovementComponent>(GetMovementComponent())->ScaleDefaultTurningSpeed(Flexibility->GetTurningSpeedMultiplier());
    }
}

void AHSSnakeCharacter::TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost)
{
    GetHSMovementComponent()->ChangeSpeedForTime(SpeedUpMultiplier, TimeOfBoost);
    bCanDie = false;
    GetWorldTimerManager().ClearTimer(ImmortalityTimerHandle);
    GetWorldTimerManager().SetTimer(ImmortalityTimerHandle, this, &AHSSnakeCharacter::SetMortal, TimeOfBoost, false);
    UE_LOG(LogSnakeCharacter, Display, TEXT("Start Immortality!"));
    UE_LOG(LogSnakeCharacter, Display, TEXT("SpeedUpMultiplier: %f"), SpeedUpMultiplier);

    SetImmortalLeather();
}

void AHSSnakeCharacter::SetImmortalLeather()
{
    if (!ImmortalLeatherMaterial) { return; }
    GetMesh()->SetMaterial(HeadLeatherMaterialIndex, ImmortalLeatherMaterial);
    TailComponent->SetMaterial(TailLeatherMaterialIndex, ImmortalLeatherMaterial);
    for (const auto SplineMesh : SplineMeshes)
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, ImmortalLeatherMaterial);
    }
}

void AHSSnakeCharacter::SetMortal()
{
    bCanDie = true;

    GetMesh()->SetMaterial(HeadLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    TailComponent->SetMaterial(TailLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    for (const auto SplineMesh : SplineMeshes)
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    }
}

AHSGrenade* AHSSnakeCharacter::CreateGrenade(TSubclassOf<AHSGrenade> GrenadeClass) const
{
    const auto Grenade = GetWorld()->SpawnActor<AHSGrenade>(GrenadeClass, GetMesh()->GetComponentTransform());
    OnCreateGrenade.Broadcast(Grenade);
    return Grenade;
}

void AHSSnakeCharacter::Death()
{
    if (!bCanDie) { return; }
    Destroy();
}

UHSHeadComponent* AHSSnakeCharacter::GetHeadComponent()
{
    return Cast<UHSHeadComponent>(GetMesh());
}

USkeletalMeshComponent* AHSSnakeCharacter::GetTailComponent()
{
    return TailComponent;
}

AHSPlayerController* AHSSnakeCharacter::GetSnakeController()
{
    return Cast<AHSPlayerController>(GetController());
}

UPrimitiveComponent* AHSSnakeCharacter::GetLastSplineMeshComponent()
{
    return SplineMeshes.Num() ? Cast<UPrimitiveComponent>(SplineMeshes.Last()) : Cast<UPrimitiveComponent>(GetHeadComponent());
}

UHSMovementComponent* AHSSnakeCharacter::GetHSMovementComponent()
{
    return HSUtils::GetHSPlayerComponent<UHSMovementComponent>(this);
}

void AHSSnakeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
