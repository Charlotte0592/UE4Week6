// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../ShootingSystem/AmmoSystem.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Setting Components
	// Gun
	GunWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunWeapon"));
	GunWeapon->SetupAttachment(GetMesh(), TEXT("Weapon"));
	// FootStep
	LeftFoot = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFoot"));
	LeftFoot->SetupAttachment(GetMesh(), TEXT("foot_l"));
	RightFoot = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFoot"));
	RightFoot->SetupAttachment(GetMesh(), TEXT("foot_r"));
	// AmmoSystem
	AmmoSystem = CreateDefaultSubobject<UAmmoSystem>(TEXT("AmmoSystem"));
	AmmoSystem->CurrentAmmo = 30;
	AmmoSystem->MaxAmmo = 30;
	AmmoSystem->AmmoInventory = 120;
	// PawnSensing
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SensingInterval = 1.0f;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 500;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsHip = false;;
	PawnSensing->OnSeePawn.AddDynamic(this, &AAICharacter::OnSeePawn);
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

float AAICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::SanitizeFloat(DamageAmount));
	if (DamageAmount >= CurrentHealth)
	{
		bIsDead = true;
	}
	else
	{
		bIsDead = false;
	}
	CurrentHealth -= DamageAmount;
	return 0.0f;
}

void AAICharacter::Fire()
{
	if (!bIsDead)
	{
		if (!bReloading)
		{
			if (!bIsHip)
			{
				if (AmmoSystem->CurrentAmmo > 0)
				{
					PlayAnimMontage(FireMontage);
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
					AmmoSystem->CurrentAmmo -= 1;

					FVector FireStart = GunWeapon->GetSocketLocation(TEXT("Muzzle"));
					FVector FireEnd = FireStart + UKismetMathLibrary::GetForwardVector(LookATRotation) * 5000;

					TArray<AActor*> ActorToIngore;
					UKismetSystemLibrary::LineTraceSingle(this, FireStart, FireEnd, ETraceTypeQuery::TraceTypeQuery2, false, ActorToIngore, EDrawDebugTrace::ForDuration, OutHit, true);
					if (UKismetSystemLibrary::IsValid(OutHit.GetActor()))
					{
						UGameplayStatics::ApplyDamage(OutHit.GetActor(), 10.0f, nullptr, this, nullptr);
						// 其他效果
					}
				}
				else
				{
					// No bullet
				}
			}
		}
	}
}

void AAICharacter::Reload()
{
	if (!bReloading)
	{
		if (AmmoSystem->CurrentAmmo < AmmoSystem->MaxAmmo && AmmoSystem->AmmoInventory > 0)
		{
			if (bIsHip)
			{
				PlayAnimMontage(ReloadMontage_Hip);
			}
			else
			{
				PlayAnimMontage(ReloadMontage_Aim);
			}
			bReloading = true;
		}
	}
}

void AAICharacter::OnSeePawn(APawn* Pawn)
{
	if (!bIsDead)
	{
		if (Pawn != nullptr)
		{
			LookATRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Pawn->GetActorLocation());
			SetActorRotation(FRotator(0, LookATRotation.Yaw, 0));
			if (AmmoSystem->CurrentAmmo == 0)
			{
				Reload();
			}
			else
			{
				Fire();
			}
		}
	}
}

void AAICharacter::printText()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::SanitizeFloat(500000));
}

