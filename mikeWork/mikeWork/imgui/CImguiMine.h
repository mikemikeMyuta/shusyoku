#pragma once

#include <DirectXMath.h>

//データをもつシングルトン
//Imguiにて使用
class IMGUIDrawdata final
{
private:
	IMGUIDrawdata() = default;
	~IMGUIDrawdata() = default;
	static IMGUIDrawdata *instance;
	static int animationSpeed;//アニメーション速度一括管理
	static bool show_another_window;
	static bool FrameAdvance;//止まっているときに進めるために使う
	static float FPS;
	static DirectX::XMFLOAT3 lightDir;
public:
	IMGUIDrawdata(const IMGUIDrawdata&) = delete;
	IMGUIDrawdata& operator=(const IMGUIDrawdata&) = delete;
	IMGUIDrawdata(IMGUIDrawdata&&) = delete;
	IMGUIDrawdata& operator=(IMGUIDrawdata&&) = delete;

	
	//ライトなどの制御も今後入れる　ifで使えるように

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


