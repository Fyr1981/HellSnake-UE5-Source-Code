// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Components/HSTailComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Player/HSSnakeCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSTailComponent, All, All)

UHSTailComponent::UHSTailComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = true;

    // Get owner's components
    if (!GetOwner()) { return; }
    SetHeadComponent(GetOwner()->GetRootComponent());
    if (const auto SnakeOwner = Cast<AHSSnakeCharacter>(GetOwner()))
    {
        SetTailTipComponent(SnakeOwner->GetTailTipComponent());
    }
}

void UHSTailComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // Update every tick TODO: Is any optimisations there?
    UpdateComponent();
}

void UHSTailComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // Server fully controls our growth
    DOREPLIFETIME(UHSTailComponent, NeedLength)
}

void UHSTailComponent::AddLength(int32 Length)
{
    if (!GetOwner()->HasAuthority() || !bCanExpansion) { return; }
    UE_LOG(LogHSTailComponent, Display, TEXT("Add length to the snake!"));
    NeedLength += Length;
    OnRep_NeedLength();
    OnAddGrowth.Broadcast(NeedLength);
}

void UHSTailComponent::UpdateComponent()
{
    UpdateSplinePoints();
    UpdateSplineMeshes();
    UpdateTailTip();
}

void UHSTailComponent::BeginPlay()
{
    Super::BeginPlay();
    checkf(HeadComponent, TEXT("Need to setup Head in TailSplineComponent"));

    if (bCanExpansion)
    {
        GetWorld()->GetTimerManager().SetTimer(GrowthTimerHandle, this, &UHSTailComponent::Growth, ExpansionRate, true);
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
}

void UHSTailComponent::OnRep_NeedLength() const
{
    GetWorld()->GetTimerManager().UnPauseTimer(GrowthTimerHandle);
}

void UHSTailComponent::Growth()
{
    if (!bCanExpansion) { return; }

    const FVector NewLocation = GetLastMesh()->GetComponentLocation()
                                - GetLastMesh()->GetForwardVector() * DistanceBetweenComponents;
    AddSplineWorldPoint(NewLocation);
    SpawnSplineMeshAndCollision();
    
    CurrentLength += 1;
    UE_LOG(LogHSTailComponent, Display, TEXT("Growthed!"));

    if (CurrentLength >= NeedLength)
    {
        GetWorld()->GetTimerManager().PauseTimer(GrowthTimerHandle);
    }
}

void UHSTailComponent::SpawnSplineMeshAndCollision()
{
    UE_LOG(LogHSTailComponent, Display, TEXT("SpawnSplineComponent!"));
    const auto SplineMesh = SpawnSplineMeshNonRegistered();
    const auto CapsuleCollision = SpawnSplineCollisionNonRegistered(SplineMesh);
    // Register only after setting up all components
    UpdateComponent();
    SplineMesh->RegisterComponent();
    CapsuleCollision->RegisterComponent();
}

UPrimitiveComponent* UHSTailComponent::GetLastMesh()
{
    return SplineMeshes.Num() ? Cast<UPrimitiveComponent>(SplineMeshes.Last()) : Cast<UPrimitiveComponent>(HeadComponent);
}

USplineMeshComponent* UHSTailComponent::SpawnSplineMeshNonRegistered()
{
    const auto SplineMesh = NewObject<USplineMeshComponent>(this);
    SplineMesh->SetMobility(EComponentMobility::Movable);
    SplineMesh->SetStaticMesh(BodyMesh);
    SplineMesh->SetAbsolute(true, true, true);
    SplineMesh->SetForwardAxis(ESplineMeshAxis::X);
    SplineMesh->bAlwaysCreatePhysicsState = false;
    SplineMeshes.Emplace(SplineMesh);
    OnSpawnSplineMesh.Broadcast(SplineMesh);
    return SplineMesh;
}

UCapsuleComponent* UHSTailComponent::SpawnSplineCollisionNonRegistered(USplineMeshComponent* SplineMesh)
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

void UHSTailComponent::UpdateSplinePoints()
{
    for (int32 i = GetNumberOfSplinePoints() - 1; i >= 0; i--)
    {
        UpdateSplinePoint(i);
    }
}

void UHSTailComponent::UpdateSplinePoint(int32 Index)
{
    FVector InLocation;
    if (Index > 0)
    {
        const float SplinePointDistance = Index * DistanceBetweenComponents;
        InLocation = GetLocationAtDistanceAlongSpline(SplinePointDistance, ESplineCoordinateSpace::World);
    }
    else
    {
        // UE_LOG(LogSlidingComponent, Warning, TEXT("Set 0 SplineMesh"))
        const auto Head = HeadComponent;
        if (!Head) { return; }
        InLocation = Head->GetComponentLocation() - Head->GetForwardVector() * DistanceBetweenHeadAndSpline;
        InLocation.Z += TailZOffset;
        //  DrawDebugPoint(GetWorld(), InLocation, 5.0f, FColor::Red, true);
    }
    SetWorldLocationAtSplinePoint(Index, InLocation);
    //  DrawDebugSphere(GetWorld(), InLocation, 180.0f, 1, FColor::Red, false);
    //  DrawDebugPoint(GetWorld(), GetSnakeOwner()->GetHeadCapsule()->GetComponentLocation(), 10.0f, FColor::Blue, false, 10.0f);
    //  DrawDebugPoint(GetWorld(), InLocation, 10.0f, FColor::Red, true);
}

void UHSTailComponent::UpdateSplineMeshes() const
{
    for (int32 i = 0; i < SplineMeshes.Num(); i++) // GetSnakeOwner()->GetSplineMeshes().Num(); i++)
    {
        //  UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh: %i"), i)
        UpdateSplineMesh(i);
    }
}

void UHSTailComponent::UpdateSplineMesh(int32 Index) const
{
    const auto SplineMesh = SplineMeshes[Index];
    if (!SplineMesh) { return; }

    const FVector StartPos = GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector StartTangent = GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World);
    const FVector EndPos = GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);
    const FVector EndTangent = GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::World);

    SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
    // _Concurrent add 20-30 fps when length of snake is near 100
    SplineMesh->UpdateMesh_Concurrent();
    UpdateBodyCollision(Index);
}

void UHSTailComponent::UpdateBodyCollision(int32 Index) const
{
    // UE_LOG(LogSlidingComponent, Warning, TEXT("Try to update SplineMesh Collision: %i"), Index)

    const auto Collision = CapsuleComponents[Index];
    if (!Collision) { return; }

    //  UE_LOG(LogSlidingComponent, Warning, TEXT("Update SplineMesh Collision: %i"), Index)

    const float CollisionDistance = Index * DistanceBetweenComponents
                                    + 0.5f * DistanceBetweenComponents; // Center of collision on center of spline mesh

    const FVector CollisionLocation = GetLocationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World);

    //TODO: another rotation calc, maybe GetRotationAtDistanceAlongSpline
    FRotator CollisionRotation;
    CollisionRotation.Pitch  = GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Pitch + 90.0;
    CollisionRotation.Yaw = GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Yaw;
    CollisionRotation.Roll = GetRotationAtDistanceAlongSpline(CollisionDistance, ESplineCoordinateSpace::World).Roll;
    Collision->SetWorldLocationAndRotation(CollisionLocation, CollisionRotation, false);
    //  DrawDebugSphere(GetWorld(), CollisionLocation, 300.0f, 5, FColor::Blue);
}

void UHSTailComponent::UpdateTailTip() const
{
    if (!TailTipComponent) { return; }
    const FVector NewLocation = GetLocationAtSplinePoint(GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::World);
    TailTipComponent->SetWorldLocation(NewLocation);
    const FRotator SplinePointRotation = GetRotationAtSplinePoint(GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::World);

    const FRotator NewRotation = FRotator(SplinePointRotation.Pitch + 180.0f, SplinePointRotation.Yaw, SplinePointRotation.Roll + 180.0f);
    //TODO: tail default relative rotation //  + SnakeOwner->GetTailRelativeRotation();
    //TODO set relative rotation
    TailTipComponent->SetWorldRotation(NewRotation);
}

UCapsuleComponent* UHSTailComponent::GetTailCollision(int32 Index)
{
    return (CapsuleComponents.Num() > Index) && (Index >= 0) ? CapsuleComponents[Index] : nullptr;
}
