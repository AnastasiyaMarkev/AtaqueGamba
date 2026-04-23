#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AttackGambaTypes.h"
#include "AttackGambaGameInstance.generated.h"

UCLASS()
class YOURPROJECT_API UAttackGambaGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UAttackGambaGameInstance();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    FSavedGameData SavedGameData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    bool bHasUnsavedProgress = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    FString ActiveSaveSlot = TEXT("SaveSlot_1");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    int32 UserIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishment")
    int32 PunishmentAmmoLoss = 10;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveToDisk(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadFromDisk(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void InitializeDefaultSaveData();

    UFUNCTION(BlueprintCallable, Category = "Save")
    FString GetFormattedSaveTimestamp() const;

    UFUNCTION(BlueprintCallable, Category = "Quit")
    bool HandleCloseRequest();

    UFUNCTION(BlueprintImplementableEvent, Category = "Quit")
    void OnCloseIntercepted();

    UFUNCTION(BlueprintCallable, Category = "Punishment")
    bool ApplyPendingPunishment();
};