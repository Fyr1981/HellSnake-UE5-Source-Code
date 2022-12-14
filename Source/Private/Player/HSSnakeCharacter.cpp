// Hell Snake! Copyright © 2022 Oleg Lando, Denis Filatov. All rights reserved.


#include "Player/HSSnakeCharacter.h"
#include "HSUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HSHeadComponent.h"
#include "Components/HSMovementComponent.h"
#include "Components/HSWeaponComponent.h"
#include "Components/HSTailComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Improvements/HSGivenGrenadeImprovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Placeables/HSGrenade.h"
#include "Player/HSPlayerController.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogHSSnakeCharacter, All, All)

AHSSnakeCharacter::AHSSnakeCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<UHSMovementComponent>(CharacterMovementComponentName).
                    SetDefaultSubobjectClass<UHSHeadComponent>(MeshComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    bUseControllerRotationYaw = false;

    WeaponComponent = CreateDefaultSubobject<UHSWeaponComponent>("WeaponComponent");

    GetCapsuleComponent()->SetCapsuleHalfHeight(80.0f);
    GetCapsuleComponent()->SetCapsuleRadius(60.0f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SnakeHeadDefault"));
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;

    TailTipComponent = CreateDefaultSubobject<USkeletalMeshComponent>("TailTipComponent");
    TailTipComponent->SetRelativeLocation(FVector(-100, 0.0, 0.0));
    TailTipComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TailComponent = CreateDefaultSubobject<UHSTailComponent>("TailComponent");

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(GetRootComponent());
    // Look down
    CameraComponent->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));
    // From a height
    CameraComponent->SetRelativeLocation(FVector(0.0, 0.0, 1500));
    CameraComponent->SetAbsolute(false, true, true);
}

void AHSSnakeCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (TailComponent)
    {
        TailComponent->UpdateComponent();
    }
}

void AHSSnakeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AHSSnakeCharacter, bCanDie);
    DOREPLIFETIME(AHSSnakeCharacter, ImprovementQueue)
}

bool AHSSnakeCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool Replicate = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    for (const auto Improvement : ImprovementQueue)
    {
        if (!Improvement) { continue; }
        Replicate |= Channel->ReplicateSubobject(Improvement, *Bunch, *RepFlags);
    }
    return Replicate;
}

void AHSSnakeCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(GetHeadComponent());
    check(GetCapsuleComponent());
    check(TailComponent);
    check(TailTipComponent);
    check(CameraComponent);
    check(WeaponComponent);

    GetHeadComponent()->OnHeadDeath.AddDynamic(this, &AHSSnakeCharacter::Death);
    GetTailComponent()->OnSpawnSplineMesh.AddDynamic(this, &AHSSnakeCharacter::OnSpawnSplineMesh);
    
    // To not be deceived by the appearance of the snake
    if (!bCanDie) { SetImmortalLeather(); }
}

void AHSSnakeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!GetSnakeController()) { return; }
    
    // Snake always try to look at mouse cursor
    const FRotator ControlRotation = UKismetMathLibrary::FindLookAtRotation(GetCapsuleComponent()->GetComponentLocation(),
        GetSnakeController()->GetLocationUnderCursor());
    GetController()->SetControlRotation(ControlRotation);
    GetMovementComponent()->AddInputVector(GetCapsuleComponent()->GetForwardVector());
}

void AHSSnakeCharacter::AddImprovements(const TArray<UHSBaseImprovement*> Improvements)
{
    UE_LOG(LogHSSnakeCharacter, Display, TEXT("Try to get improvements"));
    if (Improvements.Num() != NumOfImprovements)
    {
        UE_LOG(LogHSSnakeCharacter, Warning, TEXT("Wrong num of improvements"));
    }
    ImprovementQueue.Append(Improvements);
    OnRep_ImprovementQueue();
}

void AHSSnakeCharacter::OnRep_ImprovementQueue()
{
    UE_LOG(LogHSSnakeCharacter, Display, TEXT("Improvements replicated!"))
    if (IsImprovementQueueEmpty()) { OnImprovementsEnd.Broadcast(); }
    // it's okay if it will be excess 
    else { OnImprovementsGot.Broadcast(); }
}

void AHSSnakeCharacter::TemporaryImmortality(float SpeedUpMultiplier, float TimeOfBoost)
{
    GetHSMovementComponent()->ChangeSpeedForTime(SpeedUpMultiplier, TimeOfBoost);
    bCanDie = false;
    OnRep_bCanDie();
    GetWorldTimerManager().ClearTimer(ImmortalityTimerHandle);
    GetWorldTimerManager().SetTimer(ImmortalityTimerHandle, this, &AHSSnakeCharacter::SetMortal, TimeOfBoost, false);
    UE_LOG(LogHSSnakeCharacter, Display, TEXT("Start Immortality!"));
}

void AHSSnakeCharacter::Death()
{
    if (!bCanDie) { return; }
    Destroy();
}

void AHSSnakeCharacter::OnRep_bCanDie()
{
    if (bCanDie) { SetMortalLeather(); }
    else { SetImmortalLeather(); }
}

void AHSSnakeCharacter::SetImmortalLeather() const
{
    if (!ImmortalLeatherMaterial) { return; }
    GetMesh()->SetMaterial(HeadLeatherMaterialIndex, ImmortalLeatherMaterial);
    TailTipComponent->SetMaterial(TailLeatherMaterialIndex, ImmortalLeatherMaterial);
    for (const auto SplineMesh : TailComponent->GetMeshes())
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, ImmortalLeatherMaterial);
    }
}

void AHSSnakeCharacter::SetMortalLeather() const
{
    GetMesh()->SetMaterial(HeadLeatherMaterialIndex, TailComponent->GetBodyMesh()->GetMaterial(BodyLeatherMaterialIndex));
    TailTipComponent->SetMaterial(TailLeatherMaterialIndex, TailComponent->GetBodyMesh()->GetMaterial(BodyLeatherMaterialIndex));
    for (const auto SplineMesh : TailComponent->GetMeshes())
    {
        SplineMesh->SetMaterial(BodyLeatherMaterialIndex, TailComponent->GetBodyMesh()->GetMaterial(BodyLeatherMaterialIndex));
    }
}

void AHSSnakeCharacter::SetMortal()
{
    bCanDie = true;
    OnRep_bCanDie();
}

void AHSSnakeCharacter::CreateGrenade_Implementation(TSubclassOf<AHSGrenade> GrenadeClass)
{
    GetWorld()->SpawnActor<AHSGrenade>(GrenadeClass,
        GetMesh()->GetSocketLocation(WeaponComponent->GetWeaponSocketName()),
        FRotator::ZeroRotator);
}

void AHSSnakeCharacter::OnSpawnSplineMesh(USplineMeshComponent* SplineMeshComponent)
{
    if (!bCanDie && ImmortalLeatherMaterial)
    {
        SplineMeshComponent->SetMaterial(BodyLeatherMaterialIndex, ImmortalLeatherMaterial);
    }
}

void AHSSnakeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    InputComponent->BindAction("UseWeapon", IE_Pressed, this, &AHSSnakeCharacter::OnPrepareFire);
    InputComponent->BindAction("UseWeapon", IE_Released, this, &AHSSnakeCharacter::OnFire);

    InputComponent->BindAction("First", IE_Pressed, this, &AHSSnakeCharacter::ChooseFirstImprovement).bExecuteWhenPaused = true;
    InputComponent->BindAction("Second", IE_Pressed, this, &AHSSnakeCharacter::ChooseSecondImprovement).bExecuteWhenPaused = true;
    InputComponent->BindAction("Third", IE_Pressed, this, &AHSSnakeCharacter::ChooseThirdImprovement).bExecuteWhenPaused = true;
}

void AHSSnakeCharacter::Server_ChooseImprovement_Implementation(int32 Index)
{
    if (!(ImprovementQueue.Num() || Index >= NumOfImprovements)) { return; }
    UHSBaseImprovement* ChosenImprovement = ImprovementQueue[Index];
    if (!ChosenImprovement) { return; }
    ChosenImprovement->ApplyToSnake(this);

    // Free queue
    for (int32 i = 0; i < NumOfImprovements; i++)
    {
        // Let garbage collector do the deal
        ImprovementQueue.RemoveAt(0);
    }
    OnRep_ImprovementQueue();
    UE_LOG(LogHSSnakeCharacter, Display, TEXT("Improvement queue cleared, now its num: %i"), ImprovementQueue.Num());
}

TArray<FUIImprovementData> AHSSnakeCharacter::GetCurrentUIImprovementDatas() const
{
    TArray<FUIImprovementData> ImrovementDatas;
    if (IsImprovementQueueEmpty()) { return ImrovementDatas; }
    for (int32 i = 0; i < NumOfImprovements; i++)
    {
        if (!ImprovementQueue[i]) { continue; }
        ImrovementDatas.Add(ImprovementQueue[i]->GetUIData());
    }
    return ImrovementDatas;
}

UHSHeadComponent* AHSSnakeCharacter::GetHeadComponent()
{
    return Cast<UHSHeadComponent>(GetMesh());
}

USkeletalMeshComponent* AHSSnakeCharacter::GetTailTipComponent()
{
    return TailTipComponent;
}

AHSPlayerController* AHSSnakeCharacter::GetSnakeController()
{
    return Cast<AHSPlayerController>(GetController());
}

UHSMovementComponent* AHSSnakeCharacter::GetHSMovementComponent()
{
    return HSUtils::GetHSPlayerComponent<UHSMovementComponent>(this);
}

void AHSSnakeCharacter::OnPrepareFire()
{
    if (WeaponComponent) { WeaponComponent->InputPrepareFire(); }
}

void AHSSnakeCharacter::OnFire()
{
    if (WeaponComponent) { WeaponComponent->InputFire(); }
}

void AHSSnakeCharacter::ChooseFirstImprovement()
{
    Server_ChooseImprovement(0);
}

void AHSSnakeCharacter::ChooseSecondImprovement()
{
    Server_ChooseImprovement(1);
}

void AHSSnakeCharacter::ChooseThirdImprovement()
{
    Server_ChooseImprovement(2);
}
