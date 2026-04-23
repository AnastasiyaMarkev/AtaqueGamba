#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackGambaTypes.h"
#include "AttackGambaCharacter.generated.h"

UCLASS()
class YOURPROJECT_API AAttackGambaCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAttackGambaCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float PlayerHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MayonnaiseAmmo = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsInvulnerable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
    FVector LastValidRespawnPoint = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    EWeapon EquippedWeapon = EWeapon::MayonnaisePistol;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveToSlot(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadFromSlot(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Respawn")
    void DeathEvent();

    UFUNCTION(BlueprintCallable, Category = "Respawn")
    void RespawnAtCheckpoint();

    UFUNCTION(BlueprintCallable, Category = "Respawn")
    void UpdateRespawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Time")
    void UpdateTimePlayed();

    UFUNCTION(BlueprintCallable, Category = "Save")
    void ApplySavedDataToCharacter();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdateHUD();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void FadeIn(float Duration);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void FadeOut(float Duration);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowPunishmentMap();

private:
    FTimerHandle RespawnPointTimerHandle;
    FTimerHandle TimePlayedTimerHandle;
    FTimerHandle DeathTimerHandle;
    FTimerHandle InvulnerabilityTimerHandle;
};