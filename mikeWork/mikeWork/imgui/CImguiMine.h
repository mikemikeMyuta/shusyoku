#pragma once

#include <DirectXMath.h>

#define ANIMATION_CHAR_NUM (4)

//IMGUI�Őݒ肵���f�[�^�����V���O���g��
class IMGUIDrawdata final
{
private:
	IMGUIDrawdata() = default;
	~IMGUIDrawdata() = default;
	static IMGUIDrawdata *instance;
	static int animationSpeed;//�A�j���[�V�������x�ꊇ�Ǘ�
	static bool show_another_window;
	static bool FrameAdvance;//�~�܂��Ă���Ƃ��ɃR�}����ł��邷�邽�߂̃t���O
	static float FPS;//FPS�̐����ł��B
	static DirectX::XMFLOAT3 lightDir;
	static DirectX::XMFLOAT3 cameraPos;//�J�����̃|�W�V�����iimgui�ł��������͂Ȃ��ł��j
	static DirectX::XMFLOAT3 cameraGaze;//�J�����̒����_�iimgui�ł��������͂Ȃ��ł��j
	static  int MorphNum[ANIMATION_CHAR_NUM];//���[�t�ԍ��i�[
	static bool MorphFlag[ANIMATION_CHAR_NUM];//���[�t���s���邩�ۂ�
	static  int MorphMAXIndex[ANIMATION_CHAR_NUM];//���[�t�̍ő吔
	static  bool ModelDisplay[ANIMATION_CHAR_NUM];//���f����\�����邩

public:
	IMGUIDrawdata(const IMGUIDrawdata&) = delete;
	IMGUIDrawdata& operator=(const IMGUIDrawdata&) = delete;
	IMGUIDrawdata(IMGUIDrawdata&&) = delete;
	IMGUIDrawdata& operator=(IMGUIDrawdata&&) = delete;


	//���C�g�Ȃǂ̐������������@if�Ŏg����悤��

	static IMGUIDrawdata* get_instance()
	{
		return instance;
	}

	static void create()
	{
		if (!instance) {
			instance = new IMGUIDrawdata;
			animationSpeed = 1;
		}
	}

	static void destroy()
	{
		delete instance;
		instance = nullptr;
	}

	static int getAnimationSpeed()
	{
		return animationSpeed;
	}

	static bool getFrameAdvance()
	{
		return FrameAdvance;
	}
	static void setFPS(float data)
	{
		FPS = data;
	}

	static DirectX::XMFLOAT3 getLightDir()
	{
		return lightDir;
	}

	static void setCameraPos(DirectX::XMFLOAT3 data)
	{
		cameraPos = data;
	}

	static void setCameraGaze(DirectX::XMFLOAT3 data)
	{
		cameraGaze = data;
	}

	static void setMorphNum(unsigned int data,unsigned int num)
	{
		MorphNum[num] = data;
	}

	static unsigned int getMorphNum(unsigned int num)
	{
		return MorphNum[num];
	}

	static void setMorphFlag(bool data, unsigned int num)
	{
		MorphFlag[num] = data;
	}

	static unsigned int getMorphFlag(unsigned int num)
	{
		return MorphFlag[num];
	}

	static void setMorphMaxIndex(unsigned int data, unsigned int num)
	{
		MorphMAXIndex[num] = data;
	}

	static bool getModelDisplay(unsigned int num)
	{
		return ModelDisplay[num];
	}

	static void Draw();
};


