#include "CImguiMine.h"

IMGUIDrawdata* IMGUIDrawdata::instance;
int IMGUIDrawdata::animationSpeed;//�A�j���[�V�������x�ꊇ�Ǘ�
bool IMGUIDrawdata::show_another_window;
bool IMGUIDrawdata::FrameAdvance = true;//�~�܂��Ă���Ƃ��ɐi�߂邽�߂Ɏg��
float IMGUIDrawdata::FPS;
DirectX::XMFLOAT3 IMGUIDrawdata::lightDir = DirectX::XMFLOAT3(0, 10, -20);
DirectX::XMFLOAT3 IMGUIDrawdata::cameraPos;//�J�����̃|�W�V�����iimgui�ł��������͂Ȃ��ł��j
DirectX::XMFLOAT3 IMGUIDrawdata::cameraGaze;//�J�����̒����_�iimgui�ł��������͂Ȃ��ł��j
int IMGUIDrawdata::MorphNum[ANIMATION_CHAR_NUM];//���[�t�ԍ��i�[
bool IMGUIDrawdata::MorphFlag[ANIMATION_CHAR_NUM] = { true,true ,true ,true };//���[�t���s���邩�ۂ�
int IMGUIDrawdata::MorphMAXIndex[ANIMATION_CHAR_NUM];//���[�t�̍ő吔