/*================================================================
Filename: Actor.h
Date: 2018.2.17
Created by AirGuanZ
================================================================*/
#ifndef VW_ACTOR_H
#define VW_ACTOR_H

#include <string>

#include "../Chunk/ChunkManager.h"
#include "ActorModel.h"
#include "Camera.h"

constexpr int ACTOR_INPUT_KEY_INDEX_FRONT = 0;
constexpr int ACTOR_INPUT_KEY_INDEX_BACK  = 1 + ACTOR_INPUT_KEY_INDEX_FRONT;
constexpr int ACTOR_INPUT_KEY_INDEX_LEFT  = 1 + ACTOR_INPUT_KEY_INDEX_BACK;
constexpr int ACTOR_INPUT_KEY_INDEX_RIGHT = 1 + ACTOR_INPUT_KEY_INDEX_LEFT;
constexpr int ACTOR_INPUT_KEY_INDEX_JUMP  = 1 + ACTOR_INPUT_KEY_INDEX_RIGHT;
constexpr int ACTOR_INPUT_KEY_INDEX_UP    = 1 + ACTOR_INPUT_KEY_INDEX_JUMP;
constexpr int ACTOR_INPUT_KEY_INDEX_DOWN  = 1 + ACTOR_INPUT_KEY_INDEX_UP;
constexpr int ACTOR_INPUT_KEY_INDEX_WALK  = 1 + ACTOR_INPUT_KEY_INDEX_DOWN;

constexpr int ACTOR_INPUT_KEY_NUM         = 1 + ACTOR_INPUT_KEY_INDEX_WALK;

namespace ActorAux
{
    struct ActorParam
    {
        //�ܶ����Ը����ٶ�
        float runningAcl = 0.0008f;
        //���ߵ��Ը����ٶ�
        float walkingAcl = 0.0007f;
        //��Ծ���Ը����ٶ�
        float jumpingAcl = 0.0007f;

        //�ܶ��Ը��ٶ�����
        float runningSpeed = 0.0085f;
        //�����Ը��ٶ�����
        float walkingSpeed = 0.0045f;
        //��Ծ�Լ��ٶ�����
        float jumpingSpeed = 0.0075f;
        //�����Ը��ٶ�����
        float flyingSpeed = 0.0f;

        //վ���������ٶ�
        float standingFricAcl = 0.001f;
        //�ܶ��������ٶ�
        float runningFricAcl = 0.001f;
        //�����������ٶ�
        float walkingFricAcl = 0.001f;
        //��Ծ�������ٶ�
        float jumpingFricAcl = 0.001f;

        //��ɫ����ת���ٶ�
        float turningSpeed = 0.0166667f;

        //�������תˮƽ������
        float camMovXSpeed = 0.00075f;
        //�������ת��ֱ������
        float camMovYSpeed = 0.00075f;

        //���������ɫ�ӵ����
        float camDistance = 7.5f;
        //��ɫ�ӵ���ɫλ�õĸ߶�ƫ��
        float camDstYOffset = 1.5f;

        //������������ʱ����С��ֱ�н�
        float camDownReOffset = 0.02f;
        //������������ʱ����С��ֱ�н�
        float camUpReOffset = 0.02f;

        //��ɫ��ײ�뾶
        float collisionRadius = 0.2f;
        //��ɫ��ײ�߶�
        float collisionHeight = 1.6f;
        //ģ��λ�úͽ�ɫλ�õĴ�ֱƫ��
        float modelYOffset = 0.4f;

        //��Ծ���ٶ�
        float jumpInitVel = 0.035f;

        //�������ٶȴ�С
        float gravityAcl = 0.0002f;
        //�������ٶȷ���
        Vector3 gravityDir = Vector3(0.0f, -1.0f, 0.0f);
        //��������������ٶ�
        float gravityMaxSpeed = 0.01f;
    };

    struct UserInput
    {
        void Clear(void);

        bool keys[ACTOR_INPUT_KEY_NUM];
        Vector3 msMov;
    };

    void DefaultUserInput(UserInput &userInput);

    struct EnvirInput
    {
        void Clear(void);
    };

    //������˵Ļ�Actor::UpdateState�����ת��ҲҪ��
    //IMPROVE����״̬���������
    enum class ActorState
    {
        Standing = 0,
        Running,
        Walking,
        Jumping,
    };
}

class Actor
{
public:
    using UserInput  = ActorAux::UserInput;
    using EnvirInput = ActorAux::EnvirInput;
    using Params     = ActorAux::ActorParam;
    using State      = ActorAux::ActorState;

    bool Initialize(std::string &errMsg);

    void Render(void);

    const Matrix &GetViewProjMatrix(void) const;

    const Camera &GetCamera(void) const;

    const Vector3 &GetCameraPosition(void) const;

    const Params &GetParams(void) const;
    void SetParams(const Params &params);

    void Update(float deltaT, ChunkManager *ckMgr,
                const UserInput &uI, const EnvirInput &eI);

private:
    //������ӽ���ת
    void UpdateCameraDirection(const UserInput &uI);
    //״̬������Ӧ��
    void UpdateState(float dT, const UserInput &uI, const EnvirInput &eI);
    //�����ٶȺ���ײ���½�ɫλ��
    void UpdateActorPosition(float deltaT, ChunkManager *ckMgr);
    //���ݽ�ɫλ�á�������ӽǵȸ��������λ��
    void UpdateCameraPosition(float deltaT, ChunkManager *ckMgr);

    //״̬��ʼ����ת����

    void InitState_Standing(const UserInput &uI, const EnvirInput &eI);
    void InitState_Running (const UserInput &uI, const EnvirInput &eI);
    void InitState_Walking (const UserInput &uI, const EnvirInput &eI);
    void InitState_Jumping (const UserInput &uI, const EnvirInput &eI);

    State UpdateState_Standing(const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Running (const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Walking (const UserInput &uI, const EnvirInput &eI);
    State UpdateState_Jumping (const UserInput &uI, const EnvirInput &eI);

    void ApplyState_Standing(float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Running (float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Walking (float dT, const UserInput &uI, const EnvirInput &eI);
    void ApplyState_Jumping (float dT, const UserInput &uI, const EnvirInput &eI);

private:
    State state_;

    //��ط�
    bool onGround_;

    //λ�úͳ���
    Vector3 pos_;
    float actYaw_, dstYaw_;

    //�ٶ�
    Vector3 vel_;

    Params params_;

    Camera camera_;
    ActorModel model_;
};

#endif //VW_ACTOR_H
