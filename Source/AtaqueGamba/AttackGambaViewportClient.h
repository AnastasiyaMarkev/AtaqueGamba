#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "AttackGambaViewportClient.generated.h"

UCLASS()
class YOURPROJECT_API UAttackGambaViewportClient : public UGameViewportClient
{
    GENERATED_BODY()

public:
    virtual bool WindowCloseRequested() override;
};