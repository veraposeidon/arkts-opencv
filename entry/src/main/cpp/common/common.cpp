#include "common.h"

using namespace cv;

// 定义日志标签
static const char *TAG = "[opencv_napi]";

/**
 * 创建并返回一个 JavaScript null 值
 * @param env NAPI 环境变量
 * @return napi_value 类型的 null 值
 */
napi_value NapiGetNull(napi_env env) {
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

/**
 * 计算 OpenCV Mat 对象的数据缓冲区大小
 * @param mat OpenCV Mat 对象
 * @return 缓冲区大小（字节数）
 */
uint32_t GetMatDataBuffSize(const Mat &mat) {
    OH_LOG_Print(LOG_APP, LOG_INFO, GLOBAL_RESMGR, TAG, "Enter GetMatDataBuffSize");
    // 计算总字节数：每行字节数 * 行数
    uint32_t dataBufferSize = mat.step[0] * mat.rows;
    return dataBufferSize;
}

/**
 * 将 OpenCV Mat 对象的信息包装成 JavaScript 对象
 * @param env NAPI 环境变量
 * @param outMat 输入的 OpenCV Mat 对象
 * @param result 输出的 JavaScript 对象
 * @return 操作是否成功
 */
bool WrapJsPixelInfoInfo(napi_env env, cv::Mat &outMat, napi_value &result) {
    // 获取缓冲区大小
    uint32_t buffSize = GetMatDataBuffSize(outMat);
    
    // 设置 buffSize 属性
    napi_value value = nullptr;
    napi_create_int32(env, buffSize, &value);
    napi_set_named_property(env, result, "buffSize", value);

    // 设置 cols 属性（图像宽度）
    value = nullptr;
    napi_create_int32(env, outMat.cols, &value);
    napi_set_named_property(env, result, "cols", value);

    // 设置 rows 属性（图像高度）
    value = nullptr;
    napi_create_int32(env, outMat.rows, &value);
    napi_set_named_property(env, result, "rows", value);

    // 创建并设置 byteBuffer 属性
    void *buffer = (void *)(outMat.data);
    napi_value array;
    if (!CreateArrayBuffer(env, static_cast<uint8_t *>(buffer), buffSize, &array)) {
        napi_get_undefined(env, &result);
        OH_LOG_Print(LOG_APP, LOG_ERROR, GLOBAL_RESMGR, TAG, "CreateArrayBuffer failed!.");
        return false;
    } else {
        napi_set_named_property(env, result, "byteBuffer", array);
        OH_LOG_Print(LOG_APP, LOG_INFO, GLOBAL_RESMGR, TAG, "CreateArrayBuffer success!.");
        return true;
    }
}

/**
 * 创建 JavaScript ArrayBuffer 并复制数据
 * @param env NAPI 环境变量
 * @param src 源数据指针
 * @param srcLen 源数据长度
 * @param res 输出的 ArrayBuffer 对象
 * @return 操作是否成功
 */
bool CreateArrayBuffer(napi_env env, uint8_t *src, size_t srcLen, napi_value *res) {
    OH_LOG_Print(LOG_APP, LOG_INFO, GLOBAL_RESMGR, TAG, "Enter CreateArrayBuffer");

    // 验证输入参数
    if (src == nullptr || srcLen == 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, GLOBAL_RESMGR, TAG, "src == nullptr srcLen %{public}d", srcLen);
        return false;
    }

    // 创建 ArrayBuffer
    void *nativePtr = nullptr;
    if (napi_create_arraybuffer(env, srcLen, &nativePtr, res) != napi_ok || nativePtr == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_INFO, GLOBAL_RESMGR, TAG, "napi_create_arraybuffer failed");
        return false;
    }

    // 复制数据到 ArrayBuffer
    memcpy(nativePtr, src, srcLen);
    return true;
}