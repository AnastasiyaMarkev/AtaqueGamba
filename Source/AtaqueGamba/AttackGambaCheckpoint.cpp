#include "AttackGambaCheckpoint.h"
#include "AttackGambaCharacter.h"
#include "AttackGambaGameInstance.h"
#include "Kismet/GameplayStatics.h"

AAttackGambaCheckpoint::AAttackGambaCheckpoint()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(Root);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAttackGambaCheckpoint::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAttackGambaCheckpoint::OnCheckpointOverlap);

    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        if (GI->SavedGameData.ActivatedCheckpointIDs.Contains(CheckpointID))
        {
            bIsActivated = true;
            SetVisitedState();
        }
    }
}

void AAttackGambaCheckpoint::OnCheckpointOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (bIsActivated)
    {
        return;
    }

    AAttackGambaCharacter* PlayerCharacter = Cast<AAttackGambaCharacter>(OtherActor);
    if (!PlayerCharacter)
    {
        return;
    }

    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        bIsActivated = true;

        GI->SavedGameData.ActivatedCheckpointIDs.AddUnique(CheckpointID);
        GI->SavedGameData.bHasActiveCheckpoint = true;
        GI->SavedGameData.LastCheckpointLocation = PlayerCharacter->GetActorLocation();
        GI->SavedGameData.CheckpointLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
        GI->bHasUnsavedProgress = true;

        GI->SaveToDisk(GI->ActiveSaveSlot);
        PlayActivateFX();
    }
}