#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AttackGambaCheckpoint.generated.h"

UCLASS()
class YOURPROJECT_API AAttackGambaCheckpoint : public AActor
{
    GENERATED_BODY()

public:
    AAttackGambaCheckpoint();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> TriggerBox;

    UFUNCTION()
    void OnCheckpointOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FString CheckpointID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    bool bIsActivated = false;

    UFUNCTION(BlueprintImplementableEvent, Category = "Checkpoint")
    void SetVisitedState();

    UFUNCTION(BlueprintImplementableEvent, Category = "Checkpoint")
    void PlayActivateFX();
};
