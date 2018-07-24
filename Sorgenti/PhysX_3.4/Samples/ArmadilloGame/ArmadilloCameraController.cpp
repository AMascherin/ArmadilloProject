#include "ArmadilloCameraController.h"

using namespace SampleFramework;

// Variabili statiche locali
static PxVec3		gForce = PxVec3(0, 0, 0);
static PxVec3		gTorque = PxVec3(0, 0, 0);
static const PxReal	gLinPower = 800.0f;
static const PxReal	gJumpPower = 400.0f;
static const PxReal	gAngPower = 5000.0f;


void ArmadilloCameraController::collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents)
{
    //digital keyboard events
    DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_FORWARD, SCAN_CODE_FORWARD, XKEY_W, X1KEY_W, PS3KEY_W, PS4KEY_W, AKEY_UNKNOWN, SCAN_CODE_FORWARD, IKEY_UNKNOWN, SCAN_CODE_FORWARD, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_BACKWARD, SCAN_CODE_BACKWARD, XKEY_S, X1KEY_S, PS3KEY_S, PS4KEY_S, AKEY_UNKNOWN, SCAN_CODE_BACKWARD, IKEY_UNKNOWN, SCAN_CODE_BACKWARD, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_LEFT, SCAN_CODE_LEFT, XKEY_A, X1KEY_A, PS3KEY_A, PS4KEY_A, AKEY_UNKNOWN, SCAN_CODE_LEFT, IKEY_UNKNOWN, SCAN_CODE_LEFT, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_RIGHT, SCAN_CODE_RIGHT, XKEY_D, X1KEY_D, PS3KEY_D, PS4KEY_D, AKEY_UNKNOWN, SCAN_CODE_RIGHT, IKEY_UNKNOWN, SCAN_CODE_RIGHT, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_JUMP, SCAN_CODE_SPACE, XKEY_SPACE, X1KEY_SPACE, PS3KEY_SPACE, PS4KEY_SPACE, AKEY_UNKNOWN, OSXKEY_SPACE, IKEY_UNKNOWN, LINUXKEY_SPACE, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_DISTANCE_INCREASE, WKEY_ADD, XKEY_ADD, X1KEY_ADD, PS3KEY_ADD, PS4KEY_ADD, AKEY_UNKNOWN, OSXKEY_ADD, IKEY_UNKNOWN, LINUXKEY_ADD, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_DISTANCE_DECREASE, WKEY_SUBTRACT, XKEY_SUBTRACT, X1KEY_SUBTRACT, PS3KEY_SUBTRACT, PS4KEY_SUBTRACT, AKEY_UNKNOWN, OSXKEY_SUBTRACT, IKEY_UNKNOWN, LINUXKEY_SUBTRACT, WIIUKEY_UNKNOWN);
    DIGITAL_INPUT_EVENT_DEF(START_GAME, WKEY_Y, XKEY_SUBTRACT, X1KEY_SUBTRACT, PS3KEY_SUBTRACT, PS4KEY_SUBTRACT, AKEY_UNKNOWN, OSXKEY_SUBTRACT, IKEY_UNKNOWN, LINUXKEY_SUBTRACT, WIIUKEY_UNKNOWN);

    //digital gamepad events											
    DIGITAL_INPUT_EVENT_DEF(CAMERA_JUMP, GAMEPAD_SOUTH, GAMEPAD_SOUTH, GAMEPAD_SOUTH, GAMEPAD_SOUTH, GAMEPAD_SOUTH, AKEY_UNKNOWN, GAMEPAD_SOUTH, IKEY_UNKNOWN, LINUXKEY_UNKNOWN, GAMEPAD_SOUTH);
    DIGITAL_INPUT_EVENT_DEF(START_GAME, GAMEPAD_NORTH, GAMEPAD_NORTH, GAMEPAD_NORTH, GAMEPAD_NORTH, GAMEPAD_NORTH, AKEY_UNKNOWN, GAMEPAD_SOUTH, IKEY_UNKNOWN, LINUXKEY_UNKNOWN, GAMEPAD_SOUTH);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_DISTANCE_INCREASE, GAMEPAD_RIGHT_SHOULDER_BOT, GAMEPAD_RIGHT_SHOULDER_BOT, GAMEPAD_RIGHT_SHOULDER_BOT, GAMEPAD_RIGHT_SHOULDER_BOT, GAMEPAD_RIGHT_SHOULDER_BOT, AKEY_UNKNOWN, GAMEPAD_RIGHT_SHOULDER_BOT, IKEY_UNKNOWN, LINUXKEY_UNKNOWN, GAMEPAD_RIGHT_SHOULDER_BOT);
    DIGITAL_INPUT_EVENT_DEF(CAMERA_DISTANCE_DECREASE, GAMEPAD_LEFT_SHOULDER_BOT, GAMEPAD_LEFT_SHOULDER_BOT, GAMEPAD_LEFT_SHOULDER_BOT, GAMEPAD_LEFT_SHOULDER_BOT, GAMEPAD_LEFT_SHOULDER_BOT, AKEY_UNKNOWN, GAMEPAD_LEFT_SHOULDER_BOT, IKEY_UNKNOWN, LINUXKEY_UNKNOWN, GAMEPAD_LEFT_SHOULDER_BOT);
    //analog gamepad events
    ANALOG_INPUT_EVENT_DEF(CAMERA_GAMEPAD_ROTATE_LEFT_RIGHT, GAMEPAD_ROTATE_SENSITIVITY, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_X, LINUXKEY_UNKNOWN, GAMEPAD_RIGHT_STICK_X);
    ANALOG_INPUT_EVENT_DEF(CAMERA_GAMEPAD_ROTATE_UP_DOWN, GAMEPAD_ROTATE_SENSITIVITY, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, GAMEPAD_RIGHT_STICK_Y, LINUXKEY_UNKNOWN, GAMEPAD_RIGHT_STICK_Y);
    ANALOG_INPUT_EVENT_DEF(CAMERA_GAMEPAD_MOVE_LEFT_RIGHT, GAMEPAD_DEFAULT_SENSITIVITY, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_X, LINUXKEY_UNKNOWN, GAMEPAD_LEFT_STICK_X);
    ANALOG_INPUT_EVENT_DEF(CAMERA_GAMEPAD_MOVE_FORWARD_BACK, GAMEPAD_DEFAULT_SENSITIVITY, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, GAMEPAD_LEFT_STICK_Y, LINUXKEY_UNKNOWN, GAMEPAD_LEFT_STICK_Y);

}

void ArmadilloCameraController::onDigitalInputEvent(const SampleFramework::InputEvent & ie, bool val)
{
    switch (ie.m_Id)
    {
    case CAMERA_MOVE_FORWARD:
    {
        mFwd = val;
    }
    break;

    case CAMERA_MOVE_BACKWARD:
    {
        mBwd = val;
    }
    break;

    case CAMERA_MOVE_LEFT:
    {
        mLeft = val;
    }
    break;

    case CAMERA_MOVE_RIGHT:
    {
        mRight = val;
    }
    break;

    case CAMERA_JUMP:
    {
        mJump = val;
    }
    break;

    case START_GAME: {
        mGameStart = true;
        shdfnd::printFormatted("Starting game...");
    }
                     break;

    case CAMERA_DISTANCE_INCREASE:
    {
        mHeightUp = val;
    }
    break;

    case CAMERA_DISTANCE_DECREASE:
    {
        mHeightDown = val;
    }
    break;
    }

}

static PX_FORCE_INLINE PxReal remapAxisValue(PxReal absolutePosition)
{
    return absolutePosition * absolutePosition * absolutePosition * 5.0f;
}

void ArmadilloCameraController::onAnalogInputEvent(const SampleFramework::InputEvent & ie, float val)
{
    if (ie.m_Id == CAMERA_GAMEPAD_ROTATE_LEFT_RIGHT)
    {
        mGamepadYawInc = -remapAxisValue(val);
    }
    else if (ie.m_Id == CAMERA_GAMEPAD_ROTATE_UP_DOWN)
    {
        mGamepadPitchInc = remapAxisValue(val);
    }
    else if (ie.m_Id == CAMERA_GAMEPAD_MOVE_LEFT_RIGHT)
    {
        mGamepadLateralInc = val;
    }
    else if (ie.m_Id == CAMERA_GAMEPAD_MOVE_FORWARD_BACK)
    {
        mGamepadForwardInc = val;
    }
}

void ArmadilloCameraController::onPointerInputEvent(const SampleFramework::InputEvent &ie, physx::PxU32, physx::PxU32, physx::PxReal dx, physx::PxReal dy, bool val)
{
    if (ie.m_Id == CAMERA_MOUSE_LOOK)
    {
        mTargetYaw -= dx * mSensibility;
        mTargetPitch += dy * mSensibility;
    }
}

PxVec3 ArmadilloCameraController::getForce()
{
    return gForce;
}
void ArmadilloCameraController::resetForce()
{
    gForce = PxVec3(0);
}

bool ArmadilloCameraController::getStartGameFlag() {
    return mGameStart;
}
void ArmadilloCameraController::resetStartGameFlag() {
    mGameStart = false;
}

// Funzione per gestire gli input e calcolare le forze da applicare sul player
void ArmadilloCameraController::handleInput(Camera &camera, bool isNotJumping)
{
    PxVec3 forward = camera.getViewDir();
    forward.y = 0;
    forward.normalize();
    PxVec3 up = PxVec3(0, 1, 0);
    PxVec3 right = forward.cross(up);

    // Gestione input movimenti su tastiera (sconsigliato)
    if (mFwd) {
        gForce += gLinPower*forward*0.05;
        mFwd = false;
    }
    if (mBwd) {
        gForce -= gLinPower*forward*0.05;
        mBwd = false;
    }
    if (mRight) {
        gForce += gLinPower*right*0.05;
        mRight = false;
    }
    if (mLeft) {
        gForce -= gLinPower*right*0.05;
        mLeft = false;
    }

    // Gestione salto
    if (mJump) {
        if (isNotJumping)
            gForce.y = gJumpPower;
        mJump = false;
    }

    // Gestione zoom camera
    if (mHeightUp && mCameraDistance < 30.0f) {
        mCameraDistance += (1.0f);
    }
    if (mHeightDown && mCameraDistance > 5.0f) {
        mCameraDistance -= (1.0f);
    }

    // Gestione input movimenti da cotroller (consigliato)
    gForce += forward * mGamepadForwardInc *  mGamepadWalkingSpeed;
    gForce += right * mGamepadLateralInc * mGamepadWalkingSpeed;
}


