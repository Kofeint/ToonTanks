// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	TrailParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle Component"));
	TrailParticlesComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if(ShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ShootSound,GetActorLocation(),GetActorRotation());
	}
	ProjectileMesh->OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	UE_LOG(LogTemp,Warning,TEXT("OnHit. HitComp:%s OtherActor: %s OtherComp: %s"),*HitComp->GetName(),*OtherActor->GetName(),*OtherComp->GetName());
	
	auto MyOwner = GetOwner();
	if(MyOwner == nullptr)
	{
		Destroy();
		return;
	}

	auto MyOwnerInstigator = MyOwner->GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor,Damage,MyOwnerInstigator,MyOwner,DamageTypeClass);
		if(HitParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this,HitParticles,GetActorLocation(),GetActorRotation());
		}
	}
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitSound,OtherActor->GetActorLocation(),OtherActor->GetActorRotation());
	}
	Destroy();
}
