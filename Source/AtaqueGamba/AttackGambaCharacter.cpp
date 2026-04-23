#include "AttackGambaCharacter.h"
#include "AttackGambaGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "AttackGambaCharacter.h"

AAttackGambaCharacter::AAttackGambaCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAttackGambaCharacter::BeginPlay()
{
    Super::BeginPlay();

    FadeIn(1.0f);

    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        if (!GI->ActiveSaveSlot.IsEmpty())
        {
            GI->LoadFromDisk(GI->ActiveSaveSlot);
        }

        ApplySavedDataToCharacter();

        if (GI->ApplyPendingPunishment())
        {
            GI->SaveToDisk(GI->ActiveSaveSlot);
            ShowPunishmentMap();
        }
    }

    GetWorldTimerManager().SetTimer(
        RespawnPointTimerHandle,
        this,
        &AAttackGambaCharacter::UpdateRespawnPoint,
        1.0f,
        true
    );

    GetWorldTimerManager().SetTimer(
        TimePlayedTimerHandle,
        this,
        &AAttackGambaCharacter::UpdateTimePlayed,
        1.0f,
        true
    );

    UpdateHUD();
}

void AAttackGambaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        if (GI->bHasUnsavedProgress)
        {
            GI->SavedGameData.bPunishmentPending = true;
            GI->SaveToDisk(GI->ActiveSaveSlot);
        }
    }

    Super::EndPlay(EndPlayReason);
}

float AAttackGambaCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (bIsDead || bIsInvulnerable)
    {
        return 0.f;
    }

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    PlayerHealth = FMath::Clamp(PlayerHealth - ActualDamage, 0.f, MaxHealth);

    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        GI->SavedGameData.PlayerHealth = PlayerHealth;
        GI->bHasUnsavedProgress = true;
    }

    UpdateHUD();

    bIsInvulnerable = true;
    GetWorldTimerManager().SetTimer(
        InvulnerabilityTimerHandle,
        [this]()
        {
            bIsInvulnerable = false;
        },
        0.4f,
        false
    );

    if (PlayerHealth <= 0.f)
    {
        DeathEvent();
    }

    return ActualDamage;
}

void AAttackGambaCharacter::SaveToSlot(const FString& SlotName)
{
    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        GI->SavedGameData.PlayerHealth = PlayerHealth;
        GI->SavedGameData.MayonnaiseAmmo = MayonnaiseAmmo;
        GI->SavedGameData.EquippedWeapon = EquippedWeapon;
        GI->SavedGameData.CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);
        GI->SaveToDisk(SlotName);
    }
}

void AAttackGambaCharacter::LoadFromSlot(const FString& SlotName)
{
    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        GI->LoadFromDisk(SlotName);
        ApplySavedDataToCharacter();

        if (GI->SavedGameData.bHasActiveCheckpoint)
        {
            UGameplayStatics::OpenLevel(this, FName(*GI->SavedGameData.CheckpointLevelName));
        }
        else if (!GI->SavedGameData.CurrentLevel.IsEmpty())
        {
            UGameplayStatics::OpenLevel(this, FName(*GI->SavedGameData.CurrentLevel));
        }
    }
}

void AAttackGambaCharacter::ApplySavedDataToCharacter()
{
    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        PlayerHealth = FMath::Max(1.f, GI->SavedGameData.PlayerHealth);
        MayonnaiseAmmo = GI->SavedGameData.MayonnaiseAmmo;
        EquippedWeapon = GI->SavedGameData.EquippedWeapon;
    }
}

void AAttackGambaCharacter::DeathEvent()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    FadeOut(1.5f);

    GetWorldTimerManager().SetTimer(
        DeathTimerHandle,
        [this]()
        {
            if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
            {
                if (GI->SavedGameData.bHasActiveCheckpoint)
                {
                    RespawnAtCheckpoint();
                }
                else
                {
                    UGameplayStatics::OpenLevel(this, FName(TEXT("Menu_muerte")));
                }
            }
        },
        1.5f,
        false
    );
}

void AAttackGambaCharacter::RespawnAtCheckpoint()
{
    FVector SpawnPos = LastValidRespawnPoint;

    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        SpawnPos = GI->SavedGameData.LastCheckpointLocation;

        FHitResult Hit;
        const FVector Start = SpawnPos;
        const FVector End = SpawnPos - FVector(0.f, 0.f, 200.f);

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        const bool bHit = GetWorld()->LineTraceSingleByChannel(
            Hit,
            Start,
            End,
            ECC_Visibility,
            Params
        );

        if (bHit && Hit.GetActor() && !Hit.GetActor()->ActorHasTag(TEXT("NoRespawn")))
        {
            SpawnPos = Hit.ImpactPoint;
        }
    }

    SetActorLocation(SpawnPos);
    PlayerHealth = MaxHealth;
    bIsDead = false;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }

    FadeIn(1.0f);
    UpdateHUD();
}

void AAttackGambaCharacter::UpdateRespawnPoint()
{
    FHitResult Hit;
    const FVector Start = GetActorLocation();
    const FVector End = Start - FVector(0.f, 0.f, 150.f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    if (bHit && Hit.GetActor() && !Hit.GetActor()->ActorHasTag(TEXT("NoRespawn")))
    {
        LastValidRespawnPoint = Hit.ImpactPoint;
    }
}

void AAttackGambaCharacter::UpdateTimePlayed()
{
    if (UAttackGambaGameInstance* GI = GetGameInstance<UAttackGambaGameInstance>())
    {
        GI->SavedGameData.TimePlayed += 1.f;
        GI->bHasUnsavedProgress = true;
    }
}

