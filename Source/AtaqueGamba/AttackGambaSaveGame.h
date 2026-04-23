#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AttackGambaTypes.h"
#include "AttackGambaSaveGame.generated.h"

UCLASS()
class ATAQUEGAMBA_API UAttackGambaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FSavedGameData SaveData;
};