#include "AttackGambaViewportClient.h"
#include "AttackGambaGameInstance.h"

bool UAttackGambaViewportClient::WindowCloseRequested()
{
    if (UAttackGambaGameInstance* GI = Cast<UAttackGambaGameInstance>(GetGameInstance()))
    {
        return GI->HandleCloseRequest();
    }

    return true;
}