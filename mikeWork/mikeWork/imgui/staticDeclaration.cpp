#include "CImguiMine.h"

IMGUIDrawdata* IMGUIDrawdata::instance;
int IMGUIDrawdata::animationSpeed;//アニメーション速度一括管理
bool IMGUIDrawdata::show_another_window;
bool IMGUIDrawdata::FrameAdvance = true;//止まっているときに進めるために使う
float IMGUIDrawdata::FPS;
DirectX::XMFLOAT3 IMGUIDrawdata::lightDir = DirectX::XMFLOAT3(0, 10, -20);
DirectX::XMFLOAT3 IMGUIDrawdata::cameraPos;//カメラのポジション（imguiでいじるつもりはないです）
DirectX::XMFLOAT3 IMGUIDrawdata::cameraGaze;//カメラの注視点（imguiでいじるつもりはないです）
int IMGUIDrawdata::MorphNum[ANIMATION_CHAR_NUM];//モーフ番号格納
bool IMGUIDrawdata::MorphFlag[ANIMATION_CHAR_NUM] = { true,true ,true ,true };//モーフ実行するか否か
int IMGUIDrawdata::MorphMAXIndex[ANIMATION_CHAR_NUM];//モーフの最大数