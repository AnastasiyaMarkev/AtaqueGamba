#include "AttackGambaGameInstance.h"
#include "AttackGambaSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"

UAttackGambaGameInstance::UAttackGambaGameInstance()
{
	InitializeDefaultSaveData();
}

void UAttackGambaGameInstance::InitializeDefaultSaveData()
{
	SavedGameData = FSavedGameData{};
	SavedGameData.PlayerHealth = 100.f;
	SavedGameData.MayonnaiseAmmo = 30;
	SavedGameData.EquippedWeapon = EWeapon::MayonnaisePistol;
	SavedGameData.bHasActiveCheckpoint = false;
	SavedGameData.bPunishmentPending = false;

	bHasUnsavedProgress = false;
}

FString UAttackGambaGameInstance::GetFormattedSaveTimestamp() const
{
	return FDateTime::Now().ToString(TEXT("%d/%m/%Y %H:%M:%S"));
}

void UAttackGambaGameInstance::SaveToDisk(const FString& SlotName)
{
	UAttackGambaSaveGame* SaveObject =
		Cast<UAttackGambaSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UAttackGambaSaveGame::StaticClass())
		);

	if (!SaveObject) return;

	SaveObject->SaveData = SavedGameData;
	SaveObject->SaveData.LastSaveTimestamp = GetFormattedSaveTimestamp();

	UGameplayStatics::SaveGameToSlot(SaveObject, SlotName, UserIndex);
	bHasUnsavedProgress = false;
}

bool UAttackGambaGameInstance::LoadFromDisk(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		InitializeDefaultSaveData();
		return false;
	}

	UAttackGambaSaveGame* SaveObject =
		Cast<UAttackGambaSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)
		);

	if (!SaveObject)
	{
		InitializeDefaultSaveData();
		return false;
	}

	SavedGameData = SaveObject->SaveData;

	if (SavedGameData.PlayerHealth <= 0)
	{
		SavedGameData.PlayerHealth = 100.f;
	}

	return true;
}

bool UAttackGambaGameInstance::HandleCloseRequest()
{
	if (!bHasUnsavedProgress)
	{
		return true;
	}

	SavedGameData.bPunishmentPending = true;
	SaveToDisk(ActiveSaveSlot);

	OnCloseIntercepted();
	return false;
}

bool UAttackGambaGameInstance::ApplyPendingPunishment()
{
	if (!SavedGameData.bPunishmentPending)
	{
		return false;
	}

	SavedGameData.MayonnaiseAmmo =
		FMath::Max(0, SavedGameData.MayonnaiseAmmo - PunishmentAmmoLoss);

	SavedGameData.bPunishmentPending = false;
	bHasUnsavedProgress = true;

	return true;
}