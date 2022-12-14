// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSSlidingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HSHeadComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Math/UnitConversion.h"

DEFINE_LOG_CATEGORY_STATIC(LogSlidingComponent, All, All)

UHSSlidingComponent::UHSSlidingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHSSlidingComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentMovementSpeed = DefaultMovementSpeed;
    CurrentTurningSpeed = DefaultTurningSpeed;

    GetSnakeOwner()->GetHeadComponent()->OnSpeedUp.AddDynamic(this, &UHSSlidingComponent::ChangeSpeedForTime);
}

void UHSSlidingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    MoveAll(DeltaTime);
}

void UHSSlidingComponent::Update() const
{
    UpdateSplinePoints();
    UpdateSplineMeshes();
    UpdateTailMesh();
}

void UHSSlidingComponent::MoveAll(float DeltaTime) const
{
    if (!bCanMove) { return; }
    MoveHead(DeltaTime);
    Update();
}

void UHSSlidingComponent::MoveHead(float DeltaTime) const
{
    const auto HeadCapsule = GetSnakeOwner()->GetHeadCapsule();
    if (!HeadCapsule || !bCanMove) { return; }

    TurnHead(DeltaTime);

    const FVector StartLocation = HeadCapsule->GetComponentLocation();

    const FVector DesiredDeltaLocation = GetDirectionBeforeBlock() * CurrentMovementSpeed * DeltaTime;
    FHitResult SweepHit;
    HeadCapsule->AddWorldOffset(DesiredDeltaLocation, true, &SweepHit, ETeleportType::None);
    if (!SweepHit.IsValidBlockingHit()) { return; }
    //   UE_LOG(LogSlidingComponent, Warning, TEXT("Somewhat blocks moving!"))

    const FVector PassedDeltaLocation = HeadCapsule->GetComponentLocation() - StartLocation;
    const FVector RemainingDeltaLocation = DesiredDeltaLocation - PassedDeltaLocation;
    if (!SweepHit.GetComponent() || SweepHit.GetComponent()->Mobility == EComponentMobility::Movable)
    {
        //  UE_LOG(LogSlidingComponent, Warning, TEXT("This is movable! Go!"))
        HeadCapsule->AddWorldOffset(RemainingDeltaLocation, false);
        return;
    }

    //  UE_LOG(LogSlidingComponent, Warning, TEXT("This is non movable! Try to go upper if we can!"))

    const FVector Normal = SweepHit.ImpactNormal;
    FVector Projection = Normal.ProjectOnToNormal(HeadCapsule->GetForwardVector())
                         + Normal.ProjectOnToNormal(HeadCapsule->GetUpVector());
    Projection.Normalize();
    const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HeadCapsule->GetForwardVector(), Projection)));
    //  UE_LOG(LogSlidingComponent, Warning, TEXT("Angle: %f"), Angle)
    const FVector UpperDirection = HeadCapsule->GetForwardVector().RotateAngleAxis(Angle + 180, HeadCapsule->GetRightVector());

    HeadCapsule->AddWorldOffset(UpperDirection * RemainingDeltaLocation.Length(), true, &SweepHit, ETeleportType::None);

    // DrawDebugDirectionalArrow(GetWorld(), SweepHit.ImpactPoint,
    //     SweepHit.ImpactPoint + UpperDirection * 300.0f, 5.0f,
    //     FColor::Blue, true,
    //     5.0f, 0, 5.0f);

    // DrawDebugDirectionalArrow(GetWorld(), SweepHit.ImpactPoint, SweepHit.ImpactPoint + SweepHit.ImpactNormal * 300.0f, 5.0f,
    //     FColor::Red, true,
    //     5.0f, 0, 5.0f);
}

FVector UHSSlidingComponent::GetDirectionBeforeBlock() const
{
    const auto HeadCapsule = GetSnakeOwner()->GetHeadCapsule();
    const FVector TraceBeneathStart = HeadCapsule->GetComponentLocation()
                                      - HeadCapsule->GetUpVector() * HeadCapsule->GetScaledCapsuleHalfHeight();

    const FVector TraceBeneathEnd = HeadCapsule->GetComponentLocation()
                                    - HeadCapsule->GetUpVector() * (HeadCapsule->GetScaledCapsuleHalfHeight() + DistantToFloor);

    // DrawDebugDirectionalArrow(GetWorld(), TraceBeneathStart, TraceBeneathEnd, 5.0f,
    //     FColor::Black, true,
    //     5.0f, 0, 5.0f);

    FHitResult TraceBeneathResult;
    GetWorld()->LineTraceSingleByChannel(TraceBeneathResult, TraceBeneathStart, TraceBeneathEnd, ECC_Visibility);
    if (TraceBeneathResult.IsValidBlockingHit())
    {
        //     UE_LOG(LogSlidingComponent, Warning, TEXT("Go forward!"))
        return HeadCapsule->GetForwardVector();
    }
    //  UE_LOG(LogSlidingComponent, Warning, TEXT("There isnt roof! Go beneath"))
    return - HeadCapsule->GetUpVector();
}

void UHSSlidingComponent::TurnHead(float DeltaTime) const
{
    if (!bCanTurn || !GetSnakeOwner() || !GetSnakeOwner()->GetSnakeController()) { return; }

    const FVector HeadLocation = GetSnakeOwner()->GetHeadCapsule()->GetComponentLocation();
    const FVector DesiredDirection = GetSnakeOwner()->GetSnakeController()->GetDesiredDirection(HeadLocation);
    const FVector Direction = GetSnakeOwner()->GetHeadCapsule()->GetForwardVector();
    const float CrossProduct = FVector::CrossProduct(Direction, DesiredDirection).Z;
    const float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Direction, DesiredDirection))));
    const float MaxTurningValue = DeltaTime * CurrentTurningSpeed;
    const float AdditionalYaw = FMath::Clamp(Angle, 0.0f, MaxTurningValue) * (CrossProduct > 0.0f ? 1 : -1);
    const FRotator HeadRotation = GetSnakeOwner()->GetHeadCapsule()->GetComponentRotation();
    const FRotator NewRotation = FRotator(HeadRotation.Pitch, HeadRotation.Yaw + AdditionalYaw, HeadRotation.Roll);
    GetSnakeOwner()->GetHeadCapsule()->SetWorldRotation(NewRotation);
}

void UHSSlidingComponent::UpdateSplinePoints() const
{
    for (int32 i = GetSnakeOwner()->GetSplineComponent()->GetNumberOfSplinePoints() - 1; i >= 0; i--)
    {
        UpdateSplinePoint(i);
    }
}

void UHSSlidingComponent::UpdateSplinePoint(int32 Index) const
{
    const auto SplineComponent = GetSnakeOwner()->GetSplineComponent();
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
        InLocation = HeadCapsule->GetComponentLocation() - HeadCapsule->GetForwardVector() * DistanceBetweenHeadAndSpline;
    }
    SplineComponent->SetWorldLocationAtSplinePoint(Index, InLocation);
    //  DrawDebugSphere(GetWorld(), InLocation, 200.0f, 5, FColor::Red);
}

void UHSSlidingComponent::UpdateSplineMeshes() const
{
    for (int32 i = 0; i < GetSnakeOwner()->GetSplineMeshes().Num(); i++)
    {
        //  UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh: %i"), i)
        UpdateSplineMesh(i);
    }
}

void UHSSlidingComponent::UpdateSplineMesh(int32 Index) const
{
    const auto SplineComponent = GetSnakeOwner()->GetSplineComponent();
    const auto SplineMesh = GetSnakeOwner()->GetSplineMeshes()[Index];
    if (!SplineComponent || !SplineMesh) { return; }

    const FVector StartPos = SplineComponent->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector EndPos = SplineComponent->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);
    const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);

    SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
    SplineMesh->UpdateMesh_Concurrent();
    UpdateBodyCollision(Index);
}

void UHSSlidingComponent::UpdateTailMesh() const
{
    const auto SplineComponent = GetSnakeOwner()->GetSplineComponent();
    const auto TailMesh = GetSnakeOwner()->GetTailComponent();
    if (!SplineComponent || !TailMesh) { return; }
    const FVector NewLocation = SplineComponent->GetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 2,
        ESplineCoordinateSpace::World);
    TailMesh->SetWorldLocation(NewLocation);
    const FRotator SplinePointRotation = SplineComponent->GetRotationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 2,
        ESplineCoordinateSpace::World);
    const FRotator NewRotation = FRotator(SplinePointRotation.Pitch + 180.0, SplinePointRotation.Yaw, SplinePointRotation.Roll + 180.0);
    TailMesh->SetWorldRotation(NewRotation);
}

void UHSSlidingComponent::UpdateBodyCollision(int32 Index) const
{
    // UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh Collision: %i"), Index)
    const auto SplineComponent = GetSnakeOwner()->GetSplineComponent();
    const auto Collision = GetSnakeOwner()->GetCapsuleComponents()[Index];
    if (!SplineComponent || !Collision) { return; }

    //  UE_LOG(LogSlidingComponent, Warning, TEXT("Update SplineMesh Collision: %i"), Index)

    const float CollisionDistance = Index * GetSnakeOwner()->GetDistanceBetweenComponents()
                                    + 0.5f * GetSnakeOwner()->GetDistanceBetweenComponents();

    const FVector CollisionLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CollisionDistance,
        ESplineCoordinateSpace::World);

    FRotator CollisionRotation;
    CollisionRotation.Pitch = 90.0;
    CollisionRotation.Yaw = SplineComponent->GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Yaw;
    CollisionRotation.Roll = SplineComponent->GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Roll;
    Collision->SetWorldLocationAndRotation(CollisionLocation, CollisionRotation, false);
    //  DrawDebugSphere(GetWorld(), CollisionLocation, 300.0f, 5, FColor::Blue);
}

void UHSSlidingComponent::ScaleDefaultTurningSpeed(float Multiplier)
{
    DefaultTurningSpeed *= Multiplier;
    CurrentTurningSpeed *= Multiplier;
    UE_LOG(LogSlidingComponent, Display, TEXT("Now snake is more flexible!"));
}

void UHSSlidingComponent::ChangeSpeedForTime(float SpeedUpMultiplier, float TimeOfBoost)
{
    if (SpeedUpMultiplier < 0 || TimeOfBoost < 0) { return; }
    CurrentMovementSpeed = CurrentMovementSpeed * SpeedUpMultiplier;
    GetWorld()->GetTimerManager().ClearTimer(SpeedUpTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SpeedUpTimerHandle,
        this,
        &UHSSlidingComponent::SetupMoveAndTurnSpeedByDefault,
        TimeOfBoost,
        false);
}

void UHSSlidingComponent::StopSpeedUp()
{
    CurrentMovementSpeed = DefaultMovementSpeed;
    UE_LOG(LogSlidingComponent, Display, TEXT("SpeedUp has been stopped!"));
}

void UHSSlidingComponent::SetupMoveAndTurnSpeedByDefault()
{
    CurrentMovementSpeed = DefaultMovementSpeed;
    CurrentTurningSpeed = DefaultTurningSpeed;
}
