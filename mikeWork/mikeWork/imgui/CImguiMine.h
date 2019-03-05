#pragma once

#include <DirectXMath.h>

#define ANIMATION_CHAR_NUM (4)

//IMGUIで設定したデータを持つシングルトン
class IMGUIDrawdata final
{
private:
	IMGUIDrawdata() = default;
	~IMGUIDrawdata() = default;
	static IMGUIDrawdata *instance;
	static int animationSpeed;//アニメーション速度一括管理
	static bool show_another_window;
	static bool FrameAdvance;//止まっているときにコマ送りできるするためのフラグ
	static float FPS;//FPSの数字です。
	static DirectX::XMFLOAT3 lightDir;
	static DirectX::XMFLOAT3 cameraPos;//カメラのポジション（imguiでいじるつもりはないです）
	static DirectX::XMFLOAT3 cameraGaze;//カメラの注視点（imguiでいじるつもりはないです）
	static  int MorphNum[ANIMATION_CHAR_NUM];//モーフ番号格納
	static bool MorphFlag[ANIMATION_CHAR_NUM];//モーフ実行するか否か
	static  int MorphMAXIndex[ANIMATION_CHAR_NUM];//モーフの最大数
	static  bool ModelDisplay[ANIMATION_CHAR_NUM];//モデルを表示するか

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


