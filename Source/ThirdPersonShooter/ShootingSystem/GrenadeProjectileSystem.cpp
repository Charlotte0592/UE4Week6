// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectileSystem.h"
#include "../Characters/AICharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AGrenadeProjectileSystem::AGrenadeProjectileSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGrenadeProjectileSystem::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AGrenadeProjectileSystem::OnStop);
}

void AGrenadeProjectileSystem::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();

		if (PhysMat)
		{
			ProjectileMovement->Bounciness = PhysMat->Restitution;

			ProjectileMovement->Friction = PhysMat->Friction;
		}

		if (Cast<AAICharacter>(OtherActor))
		{
			Explode();
		}

	}
}


void AGrenadeProjectileSystem::OnStop(const FHitResult& ImpactResult)
{
	Explode();
}

// Called when the game starts or when spawned
void AGrenadeProjectileSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenadeProjectileSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

