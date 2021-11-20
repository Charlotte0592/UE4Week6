// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../ShootingSystem/AmmoSystem.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 注册摄像机手臂组件
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 把这个组件绑定到根组件
	SpringArm->SetupAttachment(RootComponent);
	// 设置摄像机手臂和根组件之间的距离
	SpringArm->TargetArmLength = 300.0f;
	// 我们使用模型组件去进行旋转,如果不设置设个的话,Pitch轴无法进行视角移动
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	// 注册摄像机组件
	TpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamera"));
	// 把摄像机绑定到摄像机手臂上
	TpsCamera->SetupAttachment(SpringArm);

	GunWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunWeapon"));
	GunWeapon->SetupAttachment(GetMesh(), TEXT("Weapon"));

	LeftFoot = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFoot"));
	LeftFoot->SetupAttachment(GetMesh(), TEXT("foot_l"));

	RightFoot = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFoot"));
	RightFoot->SetupAttachment(GetMesh(), TEXT("foot_r"));

	AmmoSystem = CreateDefaultSubobject<UAmmoSystem>(TEXT("AmmoSystem"));
	AmmoSystem->CurrentAmmo = 30;
	AmmoSystem->MaxAmmo = 30;
	AmmoSystem->AmmoInventory = 120;

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
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsHip = true;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsHip)
	{
		FVector FireStart = GunWeapon->GetSocketLocation(TEXT("Muzzle"));
		FVector FireEnd = TpsCamera->GetForwardVector() * 5000 + FireStart;

		TArray<AActor*> ActorToIngore;
		UKismetSystemLibrary::LineTraceSingle(this, FireStart, FireEnd, ETraceTypeQuery::TraceTypeQuery2, false, ActorToIngore, EDrawDebugTrace::ForOneFrame, OutHit, true);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AMyCharacter::CrouchDown);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AMyCharacter::CrouchUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMyCharacter::JumpStart);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AMyCharacter::JumpEnd);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AMyCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AMyCharacter::SprintEnd);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &AMyCharacter::IronsightDown);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Released, this, &AMyCharacter::IronsightUp);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AMyCharacter::FireDown);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AMyCharacter::FireUp);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMyCharacter::Reload);

}

float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::SanitizeFloat(DamageAmount));
	if (DamageAmount >= CurrentHealth)
	{
		bIsDead = true;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		SpringArm->SetRelativeLocation(FVector(10, 10, 70));
		SpringArm->TargetArmLength = 300;
	}
	else
	{
		bIsDead = false;
	}
	CurrentHealth -= DamageAmount;
	return 0.0f;
}

void AMyCharacter::MoveForward(float value)
{
	if (!bIsDead)
	{
		AddMovementInput(UKismetMathLibrary::GetForwardVector(GetControlRotation()), value);
		if (value < 0.5 && bIsSprinting)
		{
			bIsSprinting = false;
			UpdateMovementSpeed();
		}
	}
}

void AMyCharacter::MoveRight(float value)
{
	if (!bIsDead && !bIsSprinting)
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), value);
	}
}

void AMyCharacter::Turn(float value)
{
	if (bIsSprinting)
	{
		AddControllerYawInput(value * 0.25);
	} 
	else
	{
		AddControllerYawInput(value);
	}
}

void AMyCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AMyCharacter::CrouchDown()
{
	if (!bIsDead)
	{
		Crouch();
	}
}

void AMyCharacter::CrouchUp()
{
	if (!bIsDead)
	{
		UnCrouch();
	}
}

void AMyCharacter::JumpStart()
{
	if (!bIsDead)
	{
		Jump();
	}
}

void AMyCharacter::JumpEnd()
{
	if (!bIsDead)
	{
		StopJumping();
	}
}

void AMyCharacter::SprintStart()
{
	if (!bIsDead && bIsHip)
	{
		bIsSprinting = true;
		UnCrouch();
		UpdateMovementSpeed();
	}
}

void AMyCharacter::SprintEnd()
{
	if (!bIsDead && bIsHip)
	{
		bIsSprinting = false;
		UpdateMovementSpeed();
	}
}

void AMyCharacter::UpdateMovementSpeed()
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = 750;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 500;
	}
}

void AMyCharacter::IronsightDown()
{
	if (!bIsDead)
	{
		bIsHip = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		SpringArm->SetRelativeLocation(FVector(10, 100, 70));
		SpringArm->TargetArmLength = 200;
	}
}

void AMyCharacter::IronsightUp()
{
	if (!bIsDead)
	{
		bIsHip = true;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		SpringArm->SetRelativeLocation(FVector(10, 10, 70));
		SpringArm->TargetArmLength = 300;
	}
	OutHit.Init();
}

void AMyCharacter::FireDown()
{
	if (!bIsDead)
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &AMyCharacter::Fire, 0.15f, true, 0.f);
		// Fire();
	}
}

void AMyCharacter::FireUp()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void AMyCharacter::Fire()
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

void AMyCharacter::Reload()
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
