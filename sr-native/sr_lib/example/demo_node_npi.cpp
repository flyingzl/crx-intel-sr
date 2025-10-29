#include <napi.h>
#include "sr_lib.h" // 包含 SR 库的头文件

/**
 * @brief 开启超分辨率功能。
 * @param info N-API 回调信息
 * @return Napi::Boolean 操作是否成功
 */
Napi::Boolean TurnOnFeature(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  printf("Turning feature on...\n");

  // 注意：频繁地 Initialize/Release 可能会影响性能，
  // 理想情况下，这些操作应该在 addon 加载和卸载时执行。
  // 此处保持了原始代码的逻辑。
  SuperResolutionInitialize();
  bool success = SuperResolutionSet(true, 0); // 启用功能，并捕获返回值
  SuperResolutionRelease();

  return Napi::Boolean::New(env, success);
}

/**
 * @brief 关闭超分辨率功能。
 * @param info N-API 回调信息
 * @return Napi::Boolean 操作是否成功
 */
Napi::Boolean TurnOffFeature(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  printf("Turning feature off...\n");

  SuperResolutionInitialize();
  bool success = SuperResolutionSet(false, 0); // 禁用功能，并捕获返回值
  SuperResolutionRelease();

  return Napi::Boolean::New(env, success);
}

/**
 * @brief 获取超分辨率功能的当前状态。
 * @param info N-API 回调信息
 * @return Napi::Number 当前状态码
 */
Napi::Number GetFeatureStatus(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  printf("Getting feature status...\n");

  SuperResolutionInitialize();
  int status = SuperResolutionGetStatus(); // 调用原生库函数获取状态
  SuperResolutionRelease();

  return Napi::Number::New(env, status);
}

/**
 * @brief 初始化 addon，导出函数给 Node.js。
 */
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  // 导出 "turnOnFeature" 函数
  exports.Set(Napi::String::New(env, "turnOnFeature"), Napi::Function::New(env, TurnOnFeature));

  // 导出 "turnOffFeature" 函数
  exports.Set(Napi::String::New(env, "turnOffFeature"), Napi::Function::New(env, TurnOffFeature));

  // 导出新增的 "getFeatureStatus" 函数
  exports.Set(Napi::String::New(env, "getFeatureStatus"), Napi::Function::New(env, GetFeatureStatus));

  return exports;
}

// 注册 addon 模块
NODE_API_MODULE(srAddon, Init)