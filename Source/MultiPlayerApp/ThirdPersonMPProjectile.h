// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThirdPersonMPProjectile.generated.h"

class UParticleSystem;
class UStaticMeshComponent;
class USphereComponent;
class UProjectileMovementComponent;
//class UDamageType;

UCLASS()
class MULTIPLAYERAPP_API AThirdPersonMPProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
    // 为此Actor的属性设置默认值
    AThirdPersonMPProjectile();

    // 此投射物的基本组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* StaticMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComponent;
    UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* ExplosionEffect;

    //此投射物将造成的伤害类型和伤害。
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<UDamageType> DamageType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    float Damage;
    // Called every frame
    virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void Destroyed() override;
	

};
