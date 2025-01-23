#include "us_jni.h"
#include "api/us_sdk.h"
#include <jni.h>

// Helper functions for conversion
static void convertSDKConfig(JNIEnv *env, jobject jconfig,
                             ultra_sound::SDKConfig &config) {
  jclass configClass = env->GetObjectClass(jconfig);

  config.numWorkers = env->GetIntField(
      jconfig, env->GetFieldID(configClass, "numWorkers", "I"));
  config.modelPath = env->GetStringUTFChars(
      (jstring)env->GetObjectField(
          jconfig,
          env->GetFieldID(configClass, "modelPath", "Ljava/lang/String;")),
      nullptr);
  config.inputWidth = env->GetIntField(
      jconfig, env->GetFieldID(configClass, "inputWidth", "I"));
  config.inputHeight = env->GetIntField(
      jconfig, env->GetFieldID(configClass, "inputHeight", "I"));
  config.logPath = env->GetStringUTFChars(
      (jstring)env->GetObjectField(
          jconfig,
          env->GetFieldID(configClass, "logPath", "Ljava/lang/String;")),
      nullptr);
}

static void convertInputPacket(JNIEnv *env, jobject jinput,
                               ultra_sound::InputPacket &input) {
  jclass inputClass = env->GetObjectClass(jinput);

  // Convert string uuid
  jstring juuid = (jstring)env->GetObjectField(
      jinput, env->GetFieldID(inputClass, "uuid", "Ljava/lang/String;"));
  input.uuid = env->GetStringUTFChars(juuid, nullptr);

  // Convert other fields
  input.frameIndex =
      env->GetLongField(jinput, env->GetFieldID(inputClass, "frameIndex", "J"));

  // Convert byte array
  jbyteArray jImageData = (jbyteArray)env->GetObjectField(
      jinput, env->GetFieldID(inputClass, "imageData", "[B"));
  jsize length = env->GetArrayLength(jImageData);
  input.imageData.resize(length);
  env->GetByteArrayRegion(jImageData, 0, length,
                          reinterpret_cast<jbyte *>(input.imageData.data()));

  input.width =
      env->GetIntField(jinput, env->GetFieldID(inputClass, "width", "I"));
  input.height =
      env->GetIntField(jinput, env->GetFieldID(inputClass, "height", "I"));
  input.timestamp =
      env->GetLongField(jinput, env->GetFieldID(inputClass, "timestamp", "J"));
}

static jobject createOutputPacket(JNIEnv *env,
                                  const ultra_sound::OutputPacket &output) {
  // Create OutputPacket object
  jclass outputClass = env->FindClass("com/ultrasound/OutputPacket");
  jobject joutput = env->AllocObject(outputClass);

  // Set bboxes
  jclass bboxClass = env->FindClass("com/ultrasound/RetBBox");
  jobjectArray jbboxes =
      env->NewObjectArray(output.bboxes.size(), bboxClass, nullptr);

  for (size_t i = 0; i < output.bboxes.size(); i++) {
    jobject bbox = env->AllocObject(bboxClass);

    // Set rect array
    jintArray jrect = env->NewIntArray(4);
    env->SetIntArrayRegion(jrect, 0, 4, output.bboxes[i].rect.data());
    env->SetObjectField(bbox, env->GetFieldID(bboxClass, "rect", "[I"), jrect);

    // Set other fields
    env->SetFloatField(bbox, env->GetFieldID(bboxClass, "score", "F"),
                       output.bboxes[i].score);
    env->SetIntField(bbox, env->GetFieldID(bboxClass, "label", "I"),
                     output.bboxes[i].label);

    env->SetObjectArrayElement(jbboxes, i, bbox);
  }
  env->SetObjectField(
      joutput,
      env->GetFieldID(outputClass, "bboxes", "[Lcom/ultrasound/RetBBox;"),
      jbboxes);

  // Set frameData
  jbyteArray jframeData = env->NewByteArray(output.frameData.size());
  env->SetByteArrayRegion(
      jframeData, 0, output.frameData.size(),
      reinterpret_cast<const jbyte *>(output.frameData.data()));
  env->SetObjectField(joutput, env->GetFieldID(outputClass, "frameData", "[B"),
                      jframeData);

  // Set other fields
  env->SetObjectField(
      joutput, env->GetFieldID(outputClass, "uuid", "Ljava/lang/String;"),
      env->NewStringUTF(output.uuid.c_str()));
  env->SetLongField(joutput, env->GetFieldID(outputClass, "frameIndex", "J"),
                    output.frameIndex);
  env->SetIntField(joutput, env->GetFieldID(outputClass, "width", "I"),
                   output.width);
  env->SetIntField(joutput, env->GetFieldID(outputClass, "height", "I"),
                   output.height);
  env->SetLongField(joutput, env->GetFieldID(outputClass, "timestamp", "J"),
                    output.timestamp);

  return joutput;
}

// JNI implementations
extern "C" {

JNIEXPORT jlong JNICALL Java_com_ultrasound_UltraSound_nativeCreate(JNIEnv *,
                                                                    jobject) {
  return reinterpret_cast<jlong>(UltraSoundSDK_Create());
}

JNIEXPORT void JNICALL
Java_com_ultrasound_UltraSound_nativeDestroy(JNIEnv *, jobject, jlong handle) {
  UltraSoundSDK_Destroy(reinterpret_cast<UltraSoundSDKHandle>(handle));
}

JNIEXPORT jint JNICALL Java_com_ultrasound_UltraSound_initialize(
    JNIEnv *env, jobject, jlong handle, jobject config) {
  ultra_sound::SDKConfig sdkConfig;
  convertSDKConfig(env, config, sdkConfig);
  return static_cast<jint>(UltraSoundSDK_Initialize(
      reinterpret_cast<UltraSoundSDKHandle>(handle), &sdkConfig));
}

JNIEXPORT jint JNICALL Java_com_ultrasound_UltraSound_pushInput(JNIEnv *env,
                                                                jobject,
                                                                jlong handle,
                                                                jobject input) {
  ultra_sound::InputPacket inputPacket;
  convertInputPacket(env, input, inputPacket);
  return static_cast<jint>(UltraSoundSDK_PushInput(
      reinterpret_cast<UltraSoundSDKHandle>(handle), &inputPacket));
}

JNIEXPORT jint JNICALL Java_com_ultrasound_UltraSound_terminate(JNIEnv *,
                                                                jobject,
                                                                jlong handle) {
  return static_cast<jint>(
      UltraSoundSDK_Terminate(reinterpret_cast<UltraSoundSDKHandle>(handle)));
}

JNIEXPORT jobject JNICALL
Java_com_ultrasound_UltraSound_tryGetNext(JNIEnv *env, jobject, jlong handle) {
  ultra_sound::OutputPacket output;
  ultra_sound::ErrorCode result = UltraSoundSDK_TryGetNext(
      reinterpret_cast<UltraSoundSDKHandle>(handle), &output);

  if (result == ultra_sound::ErrorCode::SUCCESS) {
    return createOutputPacket(env, output);
  }
  return nullptr;
}

JNIEXPORT jstring JNICALL Java_com_ultrasound_UltraSound_getVersion(JNIEnv *env,
                                                                    jobject) {
  return env->NewStringUTF(UltraSoundSDK_GetVersion());
}

} // extern "C"