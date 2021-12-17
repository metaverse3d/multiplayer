// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMPProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
//#include "IDamageInterface.h"



// 设置默认值
AThirdPersonMPProjectile::AThirdPersonMPProjectile()
{
    // 将此Actor设置为每一帧调用Tick()。如果不需要，可以关闭此选项来提高性能。
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    //DamageType = UDamageType::StaticClass();
    Damage = 10.0f;

    static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
    if (DefaultExplosionEffect.Succeeded())
    {
        ExplosionEffect = DefaultExplosionEffect.Object;
    }

    //定义将作为投射物及其碰撞的根组件的SphereComponent。
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    SphereComponent->InitSphereRadius(12.5f);
    SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    RootComponent = SphereComponent;

    //在击中事件上注册此投射物撞击函数。
    if (GetLocalRole() == ROLE_Authority)
    {
        SphereComponent->OnComponentHit.AddDynamic(this, &AThirdPersonMPProjectile::OnProjectileImpact);
    }

    //定义将作为视觉呈现的网格体。
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    StaticMesh->SetupAttachment(RootComponent);

    if (DefaultMesh.Succeeded())
    {
        StaticMesh->SetStaticMesh(DefaultMesh.Object);
        StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -12.5f));
        StaticMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
    }

    //定义投射物移动组件。
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
    ProjectileMovementComponent->InitialSpeed = 1500.0f;
    ProjectileMovementComponent->MaxSpeed = 1500.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

// 当游戏开始或生成时调用
void AThirdPersonMPProjectile::BeginPlay()
{
    Super::BeginPlay();

}

void AThirdPersonMPProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor)
    {
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
    }

    Destroy();
}

void AThirdPersonMPProjectile::Destroyed()
{
    FVector spawnLocation = GetActorLocation();
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}

// 每一帧调用
void AThirdPersonMPProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

