// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* GunWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UPawnSensingComponent * PawnSensing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent * LeftFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent * RightFoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAmmoSystem* AmmoSystem;

	// ����״̬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsHip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bReloading = false;

	// ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsSprinting = false;

	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage* FireMontage;
	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage* ReloadMontage_Hip;
	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage* ReloadMontage_Aim;

	UPROPERTY(EditDefaultsOnly)
		class USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
		void Fire();

	UFUNCTION(BlueprintCallable)
		void Reload();

	UFUNCTION(BlueprintCallable)
		void OnSeePawn(APawn * Pawn);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FHitResult OutHit;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FRotator LookATRotation;
	UFUNCTION() // ������ú꣬������û��Ӧ
		void printText();

};
