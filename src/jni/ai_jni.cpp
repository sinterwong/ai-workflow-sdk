#include "api/ai_sdk.h"
#include "api/ai_types.h"
#include <android/log.h>
#include <jni.h>
#include <string>
#include <vector>

#define TAG "AIWorkflowSDK_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

std::string jstring_to_string(JNIEnv *env, jstring jstr) {
  if (!jstr)
    return "";
  const char *str = env->GetStringUTFChars(jstr, NULL);
  std::string result(str);
  env->ReleaseStringUTFChars(jstr, str);
  return result;
}

ai_workflow::SDKConfig convert_sdk_config(JNIEnv *env, jobject jconfig) {
  ai_workflow::SDKConfig config;

  jclass configClass = env->GetObjectClass(jconfig);

  jfieldID numWorkersField = env->GetFieldID(configClass, "numWorkers", "I");
  jfieldID modelRootField =
      env->GetFieldID(configClass, "modelRoot", "Ljava/lang/String;");
  jfieldID algoConfPathField =
      env->GetFieldID(configClass, "algoConfPath", "Ljava/lang/String;");
  jfieldID logPathField =
      env->GetFieldID(configClass, "logPath", "Ljava/lang/String;");
  jfieldID logLevelField = env->GetFieldID(configClass, "logLevel", "I");

  config.numWorkers = env->GetIntField(jconfig, numWorkersField);

  jstring jmodelRoot = (jstring)env->GetObjectField(jconfig, modelRootField);
  config.modelRoot = jstring_to_string(env, jmodelRoot);

  jstring jalgoConfPath =
      (jstring)env->GetObjectField(jconfig, algoConfPathField);
  config.algoConfPath = jstring_to_string(env, jalgoConfPath);

  jstring jlogPath = (jstring)env->GetObjectField(jconfig, logPathField);
  config.logPath = jstring_to_string(env, jlogPath);

  config.logLevel = env->GetIntField(jconfig, logLevelField);

  return config;
}

ai_workflow::ImageData convert_image_data(JNIEnv *env, jobject jimage) {
  ai_workflow::ImageData image;

  if (jimage == NULL) {
    LOGE("convert_image_data: jimage is NULL");
    return image;
  }

  jclass imageClass = env->GetObjectClass(jimage);

  jfieldID frameDataField = env->GetFieldID(imageClass, "frameData", "[B");
  jfieldID frameIndexField = env->GetFieldID(imageClass, "frameIndex", "J");

  jbyteArray jframeData =
      (jbyteArray)env->GetObjectField(jimage, frameDataField);
  image.frameIndex = env->GetLongField(jimage, frameIndexField);

  if (jframeData) {
    jsize length = env->GetArrayLength(jframeData);
    image.frameData.resize(length);

    jbyte *data = env->GetByteArrayElements(jframeData, NULL);
    std::copy(data, data + length, image.frameData.begin());
    env->ReleaseByteArrayElements(jframeData, data, JNI_ABORT);
  }

  return image;
}

ai_workflow::InputPacket convert_input_packet(JNIEnv *env, jobject jinput) {
  ai_workflow::InputPacket input;

  if (jinput == NULL) {
    LOGE("convert_input_packet: jinput is NULL");
    return input;
  }

  jclass inputClass = env->GetObjectClass(jinput);

  jfieldID uuidField =
      env->GetFieldID(inputClass, "uuid", "Ljava/lang/String;");
  jfieldID frameField =
      env->GetFieldID(inputClass, "frame", "Lcom/android/infer/ImageData;");
  jfieldID timestampField = env->GetFieldID(inputClass, "timestamp", "J");

  jstring juuid = (jstring)env->GetObjectField(jinput, uuidField);
  input.uuid = jstring_to_string(env, juuid);

  jobject jframe = env->GetObjectField(jinput, frameField);
  if (jframe) {
    input.frame = convert_image_data(env, jframe);
  }

  input.timestamp = env->GetLongField(jinput, timestampField);

  return input;
}

void update_rect(JNIEnv *env, jobject jrect, const ai_workflow::Rect &rect) {
  if (jrect == NULL) {
    LOGE("update_rect: jrect is NULL");
    return;
  }

  jclass rectClass = env->GetObjectClass(jrect);

  jfieldID xField = env->GetFieldID(rectClass, "x", "I");
  jfieldID yField = env->GetFieldID(rectClass, "y", "I");
  jfieldID wField = env->GetFieldID(rectClass, "w", "I");
  jfieldID hField = env->GetFieldID(rectClass, "h", "I");

  env->SetIntField(jrect, xField, rect.x);
  env->SetIntField(jrect, yField, rect.y);
  env->SetIntField(jrect, wField, rect.w);
  env->SetIntField(jrect, hField, rect.h);
}

extern "C" {

// JNI函数实现
JNIEXPORT jlong JNICALL Java_com_ai_workflow_AIWorkflowSDK_create(JNIEnv *env,
                                                                 jobject thiz) {
  LOGI("Creating AIWorkflowSDK instance");
  AIWorkflowSDKHandle handle = AIWorkflowSDK_Create();
  return reinterpret_cast<jlong>(handle);
}

JNIEXPORT void JNICALL Java_com_ai_workflow_AIWorkflowSDK_nativeDestroy(
    JNIEnv *env, jobject thiz, jlong handle) {
  LOGI("Destroying AIWorkflowSDK instance");
  if (handle == 0) {
    LOGE("Trying to destroy null handle");
    return;
  }
  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);
  AIWorkflowSDK_Destroy(nativeHandle);
}

JNIEXPORT jint JNICALL Java_com_ai_workflow_AIWorkflowSDK_initialize(
    JNIEnv *env, jobject thiz, jobject jconfig) {
  if (jconfig == NULL) {
    LOGE("initialize: jconfig is NULL");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_INPUT);
  }

  jclass thisClass = env->GetObjectClass(thiz);
  jfieldID handleField = env->GetFieldID(thisClass, "nativeHandle", "J");
  jlong handle = env->GetLongField(thiz, handleField);

  if (handle == 0) {
    LOGE("initialize: handle is 0");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_STATE);
  }

  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);
  ai_workflow::SDKConfig config = convert_sdk_config(env, jconfig);

  LOGI("Initializing AIWorkflowSDK with modelRoot: %s", config.modelRoot.c_str());
  return static_cast<jint>(AIWorkflowSDK_Initialize(nativeHandle, &config));
}

JNIEXPORT jint JNICALL Java_com_ai_workflow_AIWorkflowSDK_pushInput(
    JNIEnv *env, jobject thiz, jobject jinput) {
  if (jinput == NULL) {
    LOGE("pushInput: jinput is NULL");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_INPUT);
  }

  jclass thisClass = env->GetObjectClass(thiz);
  jfieldID handleField = env->GetFieldID(thisClass, "nativeHandle", "J");
  jlong handle = env->GetLongField(thiz, handleField);

  if (handle == 0) {
    LOGE("pushInput: handle is 0");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_STATE);
  }

  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);
  ai_workflow::InputPacket input = convert_input_packet(env, jinput);

  LOGI("Pushing input with uuid: %s, frameIndex: %ld", input.uuid.c_str(),
       input.frame.frameIndex);
  return static_cast<jint>(AIWorkflowSDK_PushInput(nativeHandle, &input));
}

JNIEXPORT jint JNICALL Java_com_ai_workflow_AIWorkflowSDK_calcCurrentROI(
    JNIEnv *env, jobject thiz, jobject jinput, jobject jroi) {
  if (jinput == NULL || jroi == NULL) {
    LOGE("calcCurrentROI: jinput or jroi is NULL");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_INPUT);
  }

  jclass thisClass = env->GetObjectClass(thiz);
  jfieldID handleField = env->GetFieldID(thisClass, "nativeHandle", "J");
  jlong handle = env->GetLongField(thiz, handleField);

  if (handle == 0) {
    LOGE("calcCurrentROI: handle is 0");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_STATE);
  }

  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);
  ai_workflow::ImageData input = convert_image_data(env, jinput);
  ai_workflow::Rect roi;

  ai_workflow::ErrorCode result =
      AIWorkflowSDK_CalcCurrentROI(nativeHandle, &input, &roi);

  if (result == ai_workflow::ErrorCode::SUCCESS) {
    update_rect(env, jroi, roi);
    LOGI("Calculated ROI: x=%d, y=%d, w=%d, h=%d", roi.x, roi.y, roi.w, roi.h);
  } else {
    LOGE("Failed to calculate ROI: %d", static_cast<int>(result));
  }

  return static_cast<jint>(result);
}

JNIEXPORT jint JNICALL Java_com_ai_workflow_AIWorkflowSDK_tryGetNextOutput(
    JNIEnv *env, jobject thiz, jobject jresult) {
  if (jresult == NULL) {
    LOGE("tryGetNextOutput: jresult is NULL");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_INPUT);
  }

  jclass thisClass = env->GetObjectClass(thiz);
  jfieldID handleField = env->GetFieldID(thisClass, "nativeHandle", "J");
  jlong handle = env->GetLongField(thiz, handleField);

  if (handle == 0) {
    LOGE("tryGetNextOutput: handle is 0");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_STATE);
  }

  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);
  ai_workflow::OutputPacket result;

  ai_workflow::ErrorCode errorCode =
      AIWorkflowSDK_TryGetNextOutput(nativeHandle, &result);

  // TODO: convert outputPacket
  return static_cast<jint>(errorCode);
}

JNIEXPORT jint JNICALL
Java_com_ai_workflow_AIWorkflowSDK_terminate(JNIEnv *env, jobject thiz) {
  jclass thisClass = env->GetObjectClass(thiz);
  jfieldID handleField = env->GetFieldID(thisClass, "nativeHandle", "J");
  jlong handle = env->GetLongField(thiz, handleField);

  if (handle == 0) {
    LOGE("terminate: handle is 0");
    return static_cast<jint>(ai_workflow::ErrorCode::INVALID_STATE);
  }

  AIWorkflowSDKHandle nativeHandle = reinterpret_cast<AIWorkflowSDKHandle>(handle);

  LOGI("Terminating AIWorkflowSDK");
  return static_cast<jint>(AIWorkflowSDK_Terminate(nativeHandle));
}

JNIEXPORT jstring JNICALL
Java_com_ai_workflow_AIWorkflowSDK_getVersion(JNIEnv *env, jclass clazz) {
  const char *version = AIWorkflowSDK_GetVersion();
  LOGI("Getting version: %s", version);
  return env->NewStringUTF(version);
}

} // extern "C"