// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonMPCharacter.generated.h"


UCLASS(config = Game)
class MULTIPLAYERAPP_API AThirdPersonMPCharacter : public ACharacter
{
    GENERATED_BODY()

        /** ��������˽���������ڽ�ɫ��� */
        
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** ��������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;
public:

    /** ���캯�� */
    AThirdPersonMPCharacter();

    /** ���Ը��� */
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** ������ת�ٶȣ���λΪ��/�롣�������ű������ܻ�Ӱ��������ת�ٶȡ�*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;

    /** ��������/�¿��ٶȣ���λΪ��/�롣�������ű������ܻ�Ӱ�������ٶȡ�*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;

protected:
    /** ��ҵ��������ֵ��������ҵ��������ֵ��Ҳ�ǳ���ʱ������ֵ��*/
    UPROPERTY(EditDefaultsOnly, Category = "Health")
        float MaxHealth;

    /** ��ҵĵ�ǰ����ֵ������0�ͱ�ʾ������*/
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float CurrentHealth;

    /** RepNotify������ͬ���Ե�ǰ����ֵ�����ĸ��ġ�*/
    UFUNCTION()
    void OnRep_CurrentHealth();

    /** ��ӦҪ���µ�����ֵ���޸ĺ������ڷ������ϵ��ã����ڿͻ����ϵ�������ӦRepNotify*/
    void OnHealthUpdate();
    void SpawnProjectile();
public:
    /** �������ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

    /** ��ǰ����ֵ��ȡֵ������*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

    /** ��ǰ����ֵ�Ĵ�ֵ����������ֵ�ķ�Χ�޶���0��MaxHealth֮�䣬������OnHealthUpdate�����ڷ������ϵ��á�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    /** �����˺����¼�����APawn���ǡ�*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    /** ��VR������HMD����*/
    void OnResetVR();

    /** ����������ǰ/������� */
    void MoveForward(float Value);

    /** �������ڲ������� */
    void MoveRight(float Value);

    /**
     * ͨ��������ã��Ը����ٶ���ת��
     * @param Rate  ���Ǳ�׼���ٶȣ���1.0��ʾ100%��������ת�ٶ�
     */
    void TurnAtRate(float Rate);

    /**
     * ͨ��������ã��Ը����ٶ�����/�¿���
     * @param Rate  ���Ǳ�׼���ٶȣ���1.0��ʾ100%��������ת�ٶ�
     */
    void LookUpAtRate(float Rate);

    /** �������뿪ʼʱʹ�õĴ������*/
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

    /** ��������ֹͣʱʹ�õĴ������*/
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
    // APawn����
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // APawn�������

    /** ��ɫҪ����ķ��������͡�*/
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
        TSubclassOf<class AThirdPersonMPProjectile> ProjectileClass;

    /** ���֮����ӳ٣���λΪ�롣���ڿ��Ʋ��Է����������ٶȣ����ɷ�ֹ������������������½�SpawnProjectileֱ�Ӱ������롣*/
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
        float FireRate;

    /** ��Ϊtrue�����������ڷ�������С�*/
    bool bIsFiringWeapon;

    /** ����������������ĺ�����Ӧ�����ɱ�����Ҵ�����*/
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
        void StartFire();

    /** ���ڽ�����������ĺ�����һ��������δ��룬��ҿ��ٴ�ʹ��StartFire��*/
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
        void StopFire();

    /** ��������Ͷ����ķ�����������*/
   // UFUNCTION(Server, Reliable)
     //   void HandleFire();

    /** ��ʱ������������ṩ���ɼ��ʱ���ڵ������ӳ١�*/
    FTimerHandle FiringTimer;

public:
    /** ����CameraBoom�Ӷ��� **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** ����FollowCamera�Ӷ��� **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};