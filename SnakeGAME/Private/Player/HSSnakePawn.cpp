// Hell Snake! Copyright � 2022 Oleg Lando, Denis Filatov. All rights reserved.

#include "Player/HSSnakePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HSPlayerController.h"
#include "Components/HSHeadComponent.h"
#include "HSCoreTypes.h"
#include "Components/HSSlidingComponent.h"
#include "Components/ShapeComponent.h"
#include "Improvements/HSFlexibilityImprovement.h"
#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Improvements/HSTempImmortalityImprovement.h"
#include "Placeables/HSGrenade.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSnakePawn, All, All)

AHSSnakePawn::AHSSnakePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    HeadCapsule = CreateDefaultSubobject<UCapsuleComponent>("HeadCapsule");
    HeadCapsule->SetupAttachment(HeadComponent);
    HeadCapsule->SetCapsuleHalfHeight(80.0f);
    HeadCapsule->SetCapsuleRadius(60.0f);
    HeadCapsule->SetCollisionProfileName(TEXT("SnakeHeadDefault"));
    HeadCapsule->SetGenerateOverlapEvents(true);
    HeadCapsule->CanCharacterStepUpOn = ECB_No;
    SetRootComponent(HeadCapsule);

    HeadComponent = CreateDefaultSubobject<UHSHeadComponent>("HeadComponent");
    HeadComponent->SetupAttachment(HeadCapsule);
    // HeadComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    TailComponent = CreateDefaultSubobject<USkeletalMeshComponent>("TailComponent");
    TailComponent->SetRelativeLocation(FVector(-DistanceBetweenComponents, 0.0, 0.0));
    TailComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //TailComponent->SetAbsolute(true, true, true);

    SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
    SplineComponent->SetAbsolute(false, true, true);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(HeadCapsule);
    // Look down
    CameraComponent->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));
    // From a height
    CameraComponent->SetRelativeLocation(FVector(0.0, 0.0, 1500));
    CameraComponent->SetAbsolute(false, true, true);

    SlidingComponent = CreateDefaultSubobject<UHSSlidingComponent>("SlidingComponent");

    BodyMesh = nullptr;
}

void AHSSnakePawn::BeginPlay()
{
    Super::BeginPlay();

    check(Cast<AHSPlayerController>(GetController()));
    check(HeadComponent);
    check(BodyMesh);
    check(CameraComponent);

    HeadComponent->OnHeadDeath.AddDynamic(this, &AHSSnakePawn::Death);
    HeadComponent->OnEatingApple.AddDynamic(this, &AHSSnakePawn::ContinueGrowth);

    if (bCanExpansion)
    {
        UE_LOG(LogSnakePawn, Display, TEXT("Timer growth!"));
        GetWorld()->GetTimerManager().SetTimer(GrowthTimerHandle, this, &AHSSnakePawn::Growth, ExpansionRate, true);
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
    if (const auto SnakeController = Cast<AHSPlayerController>(Controller))
    {
        SnakeController->OnImprovementChoice.AddDynamic(this, &AHSSnakePawn::ApplyImprovement);
    }
}

void AHSSnakePawn::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SlidingComponent->Update();
}

void AHSSnakePawn::Growth()
{
    UE_LOG(LogSnakePawn, Display, TEXT("Try growth!"));
    if (!bCanExpansion) { return; }

    bEating = true;

    const FVector NewLocation = GetLastComponent()->GetComponentLocation()
                                - GetLastComponent()->GetForwardVector() * DistanceBetweenComponents;
    SplineComponent->AddSplineWorldPoint(NewLocation);
    UE_LOG(LogSnakePawn, Display, TEXT("Added SplineWorldPoint!"));
    SpawnSplineComponent();
    CurrentLength += 1;
    UE_LOG(LogSnakePawn, Display, TEXT("Growthed!"));
    if (CurrentLength == NeedLength)
    {
        bEating = false;
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
    OnGrowth.Broadcast(GetController());
}

USplineMeshComponent* AHSSnakePawn::SpawnSplineComponent()
{
    UE_LOG(LogSnakePawn, Display, TEXT("SpawnSplineComponent!"));
    const auto SplineMesh = NewObject<USplineMeshComponent>(this);
    SplineMesh->SetMobility(EComponentMobility::Movable);
    SplineMesh->SetStaticMesh(BodyMesh);
    SplineMesh->SetAbsolute(true, true, true);
    // SplineMesh->SetStartScale()
    SplineMesh->SetForwardAxis(ESplineMeshAxis::X);

    const FVector StartPos = SplineComponent->GetLocationAtSplinePoint(CurrentLength, ESplineCoordinateSpace::World);
    const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(CurrentLength, ESplineCoordinateSpace::World);
    const FVector EndPos = SplineComponent->GetLocationAtSplinePoint(CurrentLength + 1, ESplineCoordinateSpace::World);
    const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(CurrentLength + 1, ESplineCoordinateSpace::World);
    SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
    SplineMesh->bAlwaysCreatePhysicsState = false;
    // SplineMesh->SetCollisionProfileName(TailCollisionProfileName);
    SplineMesh->RegisterComponent();

    SplineMeshes.Emplace(SplineMesh);

    const auto CapsuleCollision = NewObject<UCapsuleComponent>(this);
    CapsuleCollision->SetCapsuleSize(SplineMesh->GetStaticMesh()->GetBounds().GetBox().Max.Z,
        SplineMesh->GetStaticMesh()->GetBounds().GetBox().Max.X * 2);
    CapsuleCollision->SetCollisionProfileName(TailCollisionProfileName);
    CapsuleCollision->SetGenerateOverlapEvents(true);
    CapsuleCollision->RegisterComponent();
    CapsuleCollision->CanCharacterStepUpOn = ECB_No;
    CapsuleComponents.Emplace(CapsuleCollision);

    return SplineMesh;
}

void AHSSnakePawn::TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost)
{
    SlidingComponent->ChangeSpeedForTime(SpeedUpMultiplier, TimeOfBoost);
    bCanDie = false;
    GetWorldTimerManager().ClearTimer(ImmortalityTimerHandle);
    GetWorldTimerManager().SetTimer(ImmortalityTimerHandle, this, &AHSSnakePawn::SetMortal, TimeOfBoost, false);
    UE_LOG(LogSnakePawn, Display, TEXT("Start Immortality!"));
    UE_LOG(LogSnakePawn, Display, TEXT("SpeedUpMultiplier: %f"), SpeedUpMultiplier);

    if (!ImmortalLeatherMaterial) { return; }
    HeadComponent->SetMaterial(HeadLeatherMaterialIndex, ImmortalLeatherMaterial);
    TailComponent->SetMaterial(TailLeatherMaterialIndex, ImmortalLeatherMaterial);
    for (const auto SplineMesh : SplineMeshes)
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, ImmortalLeatherMaterial);
    }
}

AHSGrenade* AHSSnakePawn::CreateGrenade(TSubclassOf<AHSGrenade> GrenadeClass) const
{
    const auto Grenade = GetWorld()->SpawnActor<AHSGrenade>(GrenadeClass, HeadComponent->GetComponentTransform());
    OnCreateGrenade.Broadcast(Grenade);
    return Grenade;
}

void AHSSnakePawn::ApplyImprovement(UHSBaseImprovement* Improvement)
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
        SlidingComponent->ScaleDefaultTurningSpeed(Flexibility->GetTurningSpeedMultiplier());
    }
}

void AHSSnakePawn::ContinueGrowth()
{
    UE_LOG(LogSnakePawn, Display, TEXT("Try continue growth!"));
    if (bCanExpansion)
    {
        NeedLength += 1;
        if (GetWorldTimerManager().IsTimerPaused(GrowthTimerHandle))
        {
            GetWorldTimerManager().UnPauseTimer(GrowthTimerHandle);
        }
    }
}

void AHSSnakePawn::Death()
{
    if (!bCanDie) { return; }
    Destroy();
}

void AHSSnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AHSSnakePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

UPrimitiveComponent* AHSSnakePawn::GetLastComponent()
{
    return SplineMeshes.Num() ? Cast<UPrimitiveComponent>(SplineMeshes.Last()) : Cast<UPrimitiveComponent>(GetHeadComponent());
}

void AHSSnakePawn::SetMortal()
{
    bCanDie = true;

    HeadComponent->SetMaterial(HeadLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    TailComponent->SetMaterial(TailLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    for (const auto SplineMesh : SplineMeshes)
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, BodyMesh->GetMaterial(BodyLeatherMaterialIndex));
    }
}
