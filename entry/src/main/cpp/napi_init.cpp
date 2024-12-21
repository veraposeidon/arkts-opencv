#include "napi/native_api.h"
#include <string>
#include <iostream>
#include "common.h"
#include <fstream>

static napi_value Add(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    
    if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
        return nullptr;
    }

    double value0, value1;
    if (napi_get_value_double(env, args[0], &value0) != napi_ok ||
        napi_get_value_double(env, args[1], &value1) != napi_ok) {
        return nullptr;
    }

    napi_value sum;
    if (napi_create_double(env, value0 + value1, &sum) != napi_ok) {
        return nullptr;
    }
    return sum;
}

static napi_value GetHelloString(napi_env env, napi_callback_info info) {
    const std::string message = "Hello Napi";
    napi_value result;
    if (napi_create_string_utf8(env, message.c_str(), message.length(), &result) != napi_ok) {
        return nullptr;
    }
    return result;
}

using namespace std;
using namespace cv;

napi_value ProcessImage(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
        return nullptr;
    }

    // 获取输入对象的属性
    napi_value width_val, height_val, data_val;
    if (napi_get_named_property(env, args[0], "width", &width_val) != napi_ok ||
        napi_get_named_property(env, args[0], "height", &height_val) != napi_ok ||
        napi_get_named_property(env, args[0], "data", &data_val) != napi_ok) {
        return nullptr;
    }

    // 转换属性值
    int width, height;
    if (napi_get_value_int32(env, width_val, &width) != napi_ok ||
        napi_get_value_int32(env, height_val, &height) != napi_ok) {
        return nullptr;
    }

    // 获取 ArrayBuffer 数据
    void* data;
    size_t length;
    if (napi_get_arraybuffer_info(env, data_val, &data, &length) != napi_ok) {
        return nullptr;
    }

    // 图像处理
    cv::Mat inputImage(height, width, CV_8UC4, data);
    cv::Mat grayImage;
    cv::cvtColor(inputImage, grayImage, cv::COLOR_RGB2GRAY);
    cv::Mat outMat;
    cv::cvtColor(grayImage, outMat, cv::COLOR_GRAY2RGBA);
    
    // 创建返回对象
    napi_value result;
    if (napi_create_object(env, &result) != napi_ok) {
        return nullptr;
    }

    if (!WrapJsPixelInfoInfo(env, outMat, result)) {
        return nullptr;
    }
    
    return result;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"add", nullptr, Add, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getHelloString", nullptr, GetHelloString, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"processImage", nullptr, ProcessImage, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
