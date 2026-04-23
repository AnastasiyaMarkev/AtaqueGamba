#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackGambaTypes.h"
#include "AttackGambaCharacter.generated.h"

UCLASS()
class ATAQUEGAMBA_API AAttackGambaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MayonnaiseAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeapon EquippedWeapon = EWeapon::MayonnaisePistol;
};