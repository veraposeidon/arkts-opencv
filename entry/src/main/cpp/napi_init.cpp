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
    void *data;
    size_t length;
    if (napi_get_arraybuffer_info(env, data_val, &data, &length) != napi_ok) {
        return nullptr;
    }

    // 图像处理
    cv::Mat inputImage(height, width, CV_8UC4, data);
    cv::Mat grayImage;
    cv::cvtColor(inputImage, grayImage, cv::COLOR_BGRA2GRAY);
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

napi_value TransformImage(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok) {
        return nullptr;
    }

    // 获取输入参数
    napi_value width_val, height_val, data_val, src_points_val, dst_points_val, dst_width_val, dst_height_val;
    if (napi_get_named_property(env, args[0], "width", &width_val) != napi_ok ||
        napi_get_named_property(env, args[0], "height", &height_val) != napi_ok ||
        napi_get_named_property(env, args[0], "data", &data_val) != napi_ok ||
        napi_get_named_property(env, args[0], "srcPoints", &src_points_val) != napi_ok ||
        napi_get_named_property(env, args[0], "dstPoints", &dst_points_val) != napi_ok ||
        napi_get_named_property(env, args[0], "dstWidth", &dst_width_val) != napi_ok ||
        napi_get_named_property(env, args[0], "dstHeight", &dst_height_val) != napi_ok) {
        return nullptr;
    }

    // 转换基本参数
    int width, height, dst_width, dst_height;
    if (napi_get_value_int32(env, width_val, &width) != napi_ok ||
        napi_get_value_int32(env, height_val, &height) != napi_ok ||
        napi_get_value_int32(env, dst_width_val, &dst_width) != napi_ok ||
        napi_get_value_int32(env, dst_height_val, &dst_height) != napi_ok) {
        return nullptr;
    }

    // 获取图像数据
    void *data;
    size_t length;
    if (napi_get_arraybuffer_info(env, data_val, &data, &length) != napi_ok) {
        return nullptr;
    }

    // 转换源点和目标点数组
    std::vector<cv::Point2f> src_points(4);
    std::vector<cv::Point2f> dst_points(4);

    // 获取数组长度
    uint32_t src_length, dst_length;
    napi_get_array_length(env, src_points_val, &src_length);
    napi_get_array_length(env, dst_points_val, &dst_length);

    // 读取源点
    for (uint32_t i = 0; i < 4; i++) {
        napi_value point;
        napi_get_element(env, src_points_val, i, &point);
        napi_value x_val, y_val;
        napi_get_named_property(env, point, "x", &x_val);
        napi_get_named_property(env, point, "y", &y_val);
        double x, y;
        napi_get_value_double(env, x_val, &x);
        napi_get_value_double(env, y_val, &y);
        src_points[i] = cv::Point2f(x, y);
    }

    // 读取目标点
    for (uint32_t i = 0; i < 4; i++) {
        napi_value point;
        napi_get_element(env, dst_points_val, i, &point);
        napi_value x_val, y_val;
        napi_get_named_property(env, point, "x", &x_val);
        napi_get_named_property(env, point, "y", &y_val);
        double x, y;
        napi_get_value_double(env, x_val, &x);
        napi_get_value_double(env, y_val, &y);
        dst_points[i] = cv::Point2f(x, y);
    }

    // 图像处理
    cv::Mat inputImage(height, width, CV_8UC4, data);
    // 转换为 BGR 格式
    cv::Mat bgrImage;
    cv::cvtColor(inputImage, bgrImage, cv::COLOR_BGRA2BGR);

    // 进行透视变换
    cv::Mat warpedBGR;
    cv::Mat M = cv::getPerspectiveTransform(src_points, dst_points);
    cv::warpPerspective(bgrImage, warpedBGR, M, cv::Size(dst_width, dst_height));

    // 转换回 RGBA 格式
    cv::Mat outMat;
    cv::cvtColor(warpedBGR, outMat, cv::COLOR_BGR2RGBA);

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
        {"processImage", nullptr, ProcessImage, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"transformImage", nullptr, TransformImage, nullptr, nullptr, nullptr, napi_default, nullptr}};
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
