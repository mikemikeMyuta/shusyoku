#pragma once

#include <DirectXMath.h>

//�f�[�^�����V���O���g��
//Imgui�ɂĎg�p
class IMGUIDrawdata final
{
private:
	IMGUIDrawdata() = default;
	~IMGUIDrawdata() = default;
	static IMGUIDrawdata *instance;
	static int animationSpeed;//�A�j���[�V�������x�ꊇ�Ǘ�
	static bool show_another_window;
	static bool FrameAdvance;//�~�܂��Ă���Ƃ��ɐi�߂邽�߂Ɏg��
	static float FPS;
	static DirectX::XMFLOAT3 lightDir;
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
			animationSpeed = 1.0f;
		}
	}

	static void destroy()
	{
		delete instance;
		instance = nullptr;
		
	}

	static float getAnimationSpeed()
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

	static void Draw();
};


