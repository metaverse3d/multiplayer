// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMPCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "ThirdPersonMPProjectile.h"
#include "TimerManager.h"

AThirdPersonMPCharacter::AThirdPersonMPCharacter()
{
    // ������ײ������Ĵ�С
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // �����������ת�ٶ�
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // ��������תʱ����ת��ֻӰ���������
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ���ý�ɫ�ƶ�
    GetCharacterMovement()->bOrientRotationToMovement = true; // ��ɫ������ķ����ƶ�...  
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...���ô���ת�ٶ�
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // ������������ˣ�������ײʱ�����������
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // ����������������ڽ�ɫ��� 
    CameraBoom->bUsePawnControlRotation = true; // ���ڿ�������ת����

    // ������������ͷ
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // ����������ӵ�����ĩ�ˣ����ڵ�����ƥ�����������
    FollowCamera->bUsePawnControlRotation = false; // �����������ڵ�����ת

    // ע�⣺�Ǽ������������������ϵĶ�����ͼ���ã��̳��Խ�ɫ�� 
    // ������ΪMyCharacter��������ͼ�ʲ������ã��Ա���C++�����µ�ֱ���������ã�

    //��ʼ���������ֵ
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    //��ʼ��Ͷ������
    ProjectileClass = AThirdPersonMPProjectile::StaticClass();
    //��ʼ������
    FireRate = 0.25f;
    bIsFiringWeapon = false;
}

//////////////////////////////////////////////////////////////////////////
// ����

void AThirdPersonMPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // ������Ϸ���̼���
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonMPCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonMPCharacter::MoveRight);

    // ������������ת�󶨰汾�������ò�ͬ�ķ�ʽ����ͬ���͵��豸
    // "turn"�����ṩ�����������豸��
    // "turnrate"����ѡ����Ϊ�仯�ٶȵ��豸������ģ����ݸ�
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AThirdPersonMPCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AThirdPersonMPCharacter::LookUpAtRate);

    // �������豸
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AThirdPersonMPCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AThirdPersonMPCharacter::TouchStopped);

    // VRͷ���豸����
    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AThirdPersonMPCharacter::OnResetVR);

    // ������Ͷ����
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AThirdPersonMPCharacter::StartFire);
    //PlayerInputComponent->BindAction("Fire", IE_Released, this, &AThirdPersonMPCharacter::StopFire);
}

//////////////////////////////////////////////////////////////////////////
// ���Ƶ�����

void AThirdPersonMPCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AThirdPersonMPCharacter, CurrentHealth);
}

void AThirdPersonMPCharacter::StartFire()
{
    if (!bIsFiringWeapon)
    {
        bIsFiringWeapon = true;
        UWorld* World = GetWorld();
        World->GetTimerManager().SetTimer(FiringTimer, this, &AThirdPersonMPCharacter::StopFire, FireRate, false);
        SpawnProjectile();
    }
}

void AThirdPersonMPCharacter::StopFire()
{
    bIsFiringWeapon = false;
}

void AThirdPersonMPCharacter::SpawnProjectile()
{
    FVector spawnLocation = GetActorLocation() + (GetActorRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
    FRotator spawnRotation = GetActorRotation();

    FActorSpawnParameters spawnParameters;
    spawnParameters.Instigator = GetInstigator();
    spawnParameters.Owner = this;

    AThirdPersonMPProjectile* spawnedProjectile = GetWorld()->SpawnActor<AThirdPersonMPProjectile>(ProjectileClass, spawnLocation, spawnRotation, spawnParameters);
}

void AThirdPersonMPCharacter::OnHealthUpdate()
{
    //�ͻ����ض��Ĺ���
    if (IsLocallyControlled())
    {
        FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

        if (CurrentHealth <= 0)
        {
            FString deathMessage = FString::Printf(TEXT("You have been killed."));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
        }
    }

    //�������ض��Ĺ���
    if (GetLocalRole() == ROLE_Authority)
    {
        FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
    }

    //�����л����϶�ִ�еĺ����� 
    /*
        ���κ����˺������������������⹦�ܶ�Ӧ�������
    */
}

void AThirdPersonMPCharacter::OnRep_CurrentHealth()
{
    OnHealthUpdate();
}

void AThirdPersonMPCharacter::SetCurrentHealth(float healthValue)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
        OnHealthUpdate();
    }
}

float AThirdPersonMPCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float damageApplied = CurrentHealth - DamageTaken;
    SetCurrentHealth(damageApplied);
    return damageApplied;
}

void AThirdPersonMPCharacter::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AThirdPersonMPCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    Jump();
}

void AThirdPersonMPCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    StopJumping();
}

void AThirdPersonMPCharacter::TurnAtRate(float Rate)
{
    // �����ٶ���Ϣ�����֡������
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonMPCharacter::LookUpAtRate(float Rate)
{
    // �����ٶ���Ϣ�����֡������
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonMPCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // ��ȡǰ��ʸ��
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonMPCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // �ҳ���ȷ�ĵ�·
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // ��ȡ��ȷ��ʸ�� 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}
