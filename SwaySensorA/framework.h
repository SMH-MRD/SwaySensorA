// header.h : 標準のシステム インクルード ファイルのインクルード ファイル、
// またはプロジェクト専用のインクルード ファイル
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#include <windows.h>
// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// プログラムに必要な追加ヘッダー
#include <string>
#include <commctrl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/BaslerUniversalInstantCamera.h>

using namespace std;
using namespace cv;
using namespace GenApi;
using namespace Pylon;

//iostream用
#include <iostream>
#include <iomanip>
#include <sstream> //stringstream用
#include <vector>
#include <stdio.h>

//マルチメディアタイマー用
#include <mmsystem.h>


//アプリケーション共通フォーマット、CODE定義ヘッダ
#include "AppCommon.h"

