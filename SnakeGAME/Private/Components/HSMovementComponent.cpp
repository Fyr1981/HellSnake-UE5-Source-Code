// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSMovementComponent.h"
#include "HSUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Player/HSPlayerController.h"
#include "Components/HSHeadComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/HSSnakeCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSMovementComponent, All, All)

UHSMovementComponent::UHSMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    MaxWalkSpeed = 860.0f;
    RotationRate.Yaw = 250.0f;
    bUseControllerDesiredRotation = true;
}

void UHSMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    SetupPointers();
    check(IsSetupped());

    DefaultMovementSpeed = MaxWalkSpeed;
    DefaultYawSpeed = RotationRate.Yaw;

    GetSnakeOwner()->GetHeadComponent()->OnSpeedUp.AddDynamic(this, &UHSMovementComponent::ChangeSpeedForTime);
}

void UHSMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    MoveAll(DeltaTime);
}

void UHSMovementComponent::MoveAll(float DeltaTime)
{
    if (!bCanMove) { return; }
    MoveHead(DeltaTime);
    Update();
}

void UHSMovementComponent::Update()
{
    if (!IsSetupped()) { SetupPointers(); }
    UpdateSplinePoints();
    UpdateSplineMeshes();
    UpdateTailMesh();
}

void UHSMovementComponent::MoveHead(float DeltaTime)
{
    if (!IsSetupped()) { return; }
    const auto HeadCapsule = SnakeOwner->GetCapsuleComponent(); //GetSnakeOwner()->GetHeadCapsule();
    if (!HeadCapsule || !bCanMove || !GetController()) { return; }

    const FRotator ControlRotation = UKismetMathLibrary::FindLookAtRotation(HeadCapsule->GetComponentLocation(),
        GetSnakeOwner()->GetSnakeController()->GetLocationUnderCursor());

    GetController()->SetControlRotation(ControlRotation);
    AddInputVector(HeadCapsule->GetForwardVector());
}

void UHSMovementComponent::UpdateSplinePoints() const
{
    if (!IsSetupped()) { return; }
    for (int32 i = OwnerSplineComponent->GetNumberOfSplinePoints() - 1; i >= 0; i--)
    {
        UpdateSplinePoint(i);
    }
}

void UHSMovementComponent::UpdateSplinePoint(int32 Index) const
{
    const auto SplineComponent = OwnerSplineComponent; //GetSnakeOwner()->GetSplineComponent();
    if (!SplineComponent) { return; }

    FVector InLocation;
    if (Index > 0)
    {
        const float SplinePointDistance = Index * GetSnakeOwner()->GetDistanceBetweenComponents();
        InLocation = SplineComponent->GetLocationAtDistanceAlongSpline(SplinePointDistance, ESplineCoordinateSpace::World);
    }
    else
    {
        // UE_LOG(LogSlidingComponent, Warning, TEXT("Set 0 SplineMesh"))
        const auto HeadCapsule = GetSnakeOwner()->GetHeadCapsule();
        if (!HeadCapsule) { return; }
        InLocation = HeadCapsule->GetComponentLocation() -
                     HeadCapsule->GetForwardVector() * GetSnakeOwner()->GetDistanceBetweenHeadAndSpline();
        InLocation.Z += GetSnakeOwner()->GetTailZOffset();
        //  DrawDebugPoint(GetWorld(), InLocation, 5.0f, FColor::Red, true);
    }
    SplineComponent->SetWorldLocationAtSplinePoint(Index, InLocation);
    //  DrawDebugSphere(GetWorld(), InLocation, 180.0f, 1, FColor::Red, false);
    //  DrawDebugPoint(GetWorld(), GetSnakeOwner()->GetHeadCapsule()->GetComponentLocation(), 10.0f, FColor::Blue, false, 10.0f);
    //  DrawDebugPoint(GetWorld(), InLocation, 10.0f, FColor::Red, true);
}

void UHSMovementComponent::UpdateSplineMeshes() const
{
    if (!IsSetupped()) { return; }
    for (int32 i = 0; i < SnakeOwner->GetSplineMeshes().Num(); i++) // GetSnakeOwner()->GetSplineMeshes().Num(); i++)
    {
        //  UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh: %i"), i)
        UpdateSplineMesh(i);
    }
}

void UHSMovementComponent::UpdateSplineMesh(int32 Index) const
{
    const auto SplineComponent = OwnerSplineComponent;            //GetSnakeOwner()->GetSplineComponent();
    const auto SplineMesh = SnakeOwner->GetSplineMeshes()[Index]; //GetSnakeOwner()->GetSplineMeshes()[Index];
    if (!SplineComponent || !SplineMesh) { return; }

    const FVector StartPos = SplineComponent->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector EndPos = SplineComponent->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);
    const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);

    SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
    // Concurrent add 20-30 fps when length of snake is near 100
    SplineMesh->UpdateMesh_Concurrent();
    UpdateBodyCollision(Index);
}

void UHSMovementComponent::UpdateTailMesh() const
{
    if (!IsSetupped()) { return; }
    const auto SplineComponent = OwnerSplineComponent;    // GetSnakeOwner()->GetSplineComponent();
    const auto TailMesh = SnakeOwner->GetTailComponent(); //GetSnakeOwner()->GetTailComponent();
    if (!SplineComponent || !TailMesh) { return; }
    const FVector NewLocation = SplineComponent->GetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 2,
        ESplineCoordinateSpace::World);
    TailMesh->SetWorldLocation(NewLocation);
    const FRotator SplinePointRotation = SplineComponent->GetRotationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 2,
        ESplineCoordinateSpace::World);
    const FRotator NewRotation = FRotator(SplinePointRotation.Pitch, SplinePointRotation.Yaw, SplinePointRotation.Roll)
                                 + SnakeOwner->GetTailRelativeRotation();
    TailMesh->SetWorldRotation(NewRotation);
}

void UHSMovementComponent::UpdateBodyCollision(int32 Index) const
{
    // UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh Collision: %i"), Index)
    if (!IsSetupped()) { return; }
    const auto SplineComponent = OwnerSplineComponent;                //GetSnakeOwner()->GetSplineComponent();
    const auto Collision = SnakeOwner->GetCapsuleComponents()[Index]; //GetSnakeOwner()->GetCapsuleComponents()[Index];
    if (!SplineComponent || !Collision) { return; }

    //  UE_LOG(LogSlidingComponent, Warning, TEXT("Update SplineMesh Collision: %i"), Index)

    const float CollisionDistance = Index * SnakeOwner->GetDistanceBetweenComponents()   // GetSnakeOwner()->GetDistanceBetweenComponents()
                                    + 0.5f * SnakeOwner->GetDistanceBetweenComponents(); // GetSnakeOwner()->GetDistanceBetweenComponents();

    const FVector CollisionLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CollisionDistance,
        ESplineCoordinateSpace::World);

    FRotator CollisionRotation;
    CollisionRotation.Pitch = 90.0;
    CollisionRotation.Yaw = SplineComponent->GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Yaw;
    CollisionRotation.Roll = SplineComponent->GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Roll;
    Collision->SetWorldLocationAndRotation(CollisionLocation, CollisionRotation, false);
    //  DrawDebugSphere(GetWorld(), CollisionLocation, 300.0f, 5, FColor::Blue);
}

void UHSMovementComponent::ScaleDefaultTurningSpeed(float Multiplier)
{
    DefaultYawSpeed *= Multiplier;
    RotationRate.Yaw = DefaultYawSpeed;
    UE_LOG(LogHSMovementComponent, Display, TEXT("Now snake is more flexible!"));
}

void UHSMovementComponent::ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost)
{
    if (SpeedUpMultiplier < 0 || TimeOfBoost < 0) { return; }
    MaxWalkSpeed *= SpeedUpMultiplier;
    GetWorld()->GetTimerManager().ClearTimer(SpeedUpTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SpeedUpTimerHandle,
        this,
        &UHSMovementComponent::SetupMoveAndTurnSpeedByDefault,
        TimeOfBoost,
        false);
}

void UHSMovementComponent::StopSpeedUp()
{
    MaxWalkSpeed = DefaultMovementSpeed;
    UE_LOG(LogHSMovementComponent, Display, TEXT("SpeedUp has been stopped!"));
}

void UHSMovementComponent::SetupMoveAndTurnSpeedByDefault()
{
    MaxWalkSpeed = DefaultMovementSpeed;
    RotationRate.Yaw = DefaultYawSpeed;
}

AHSSnakeCharacter* UHSMovementComponent::GetSnakeOwner() const
{
    return Cast<AHSSnakeCharacter>(GetOwner());
}

bool UHSMovementComponent::IsSetupped() const
{
    return SnakeOwner && OwnerSplineComponent;
}

void UHSMovementComponent::SetupPointers()
{
    SnakeOwner = Cast<AHSSnakeCharacter>(GetOwner());
    OwnerSplineComponent = SnakeOwner->GetSplineComponent();
}
