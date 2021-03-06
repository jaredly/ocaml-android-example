#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/printexc.h>
#include <caml/threads.h>
#include <caml/bigarray.h>

#include <android/log.h>
#include <android/asset_manager_jni.h>
// #include <png.h>
// #include <GLES3/gl3.h>
#include <GLES2/gl2.h>

#include <jni.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "reasongl",__VA_ARGS__)

// Init the runtime
__attribute__ ((__constructor__))
void reasongl_init(void) {
    char *caml_argv[1] = { NULL };
    caml_startup(caml_argv);
}

CAMLprim void logAndroid(value text) {
  CAMLparam1(text);
  char* str_text = String_val(text);

  __android_log_print(ANDROID_LOG_INFO, "reasongl", str_text);
  CAMLreturn0;
}

CAMLprim void showAlert(value ocamlWindow, value title, value message) {
  CAMLparam3(ocamlWindow, title, message);
  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jstring javaTitle = (*g_env)->NewStringUTF(g_env, String_val(title));
  jstring javaMessage = (*g_env)->NewStringUTF(g_env, String_val(message));
  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "showAlert", "(Ljava/lang/String;Ljava/lang/String;)V");
  (*g_env)->CallVoidMethod(g_env, g_pngmgr, method, javaTitle, javaMessage);

  CAMLreturn0;
}

CAMLprim void startHotReloading(value ocamlWindow, value host, value baseFile) {
  CAMLparam3(ocamlWindow, host, baseFile);
  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jstring javaHost = (*g_env)->NewStringUTF(g_env, String_val(host));
  jstring javaBaseFile = (*g_env)->NewStringUTF(g_env, String_val(baseFile));
  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "startHotReloading", "(Ljava/lang/String;Ljava/lang/String;)V");
  (*g_env)->CallVoidMethod(g_env, g_pngmgr, method, javaHost, javaBaseFile);

  CAMLreturn0;
}

CAMLprim value getWindowHeight(value ocamlWindow) {
  CAMLparam1(ocamlWindow);
  JNIEnv* env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject glView = (jobject)(void *)Field(ocamlWindow, 1);

  jclass viewClass = (*env)->GetObjectClass(env, glView);
  // TODO account for the hardware buttons
  jmethodID getHeight = (*env)->GetMethodID(env, viewClass, "getHeight", "()I");
  int height =  (*env)->CallIntMethod(env, glView, getHeight);
  LOGI("========= window height %d", height);

  CAMLreturn(Val_int(height));
  // CAMLreturn(Val_int(1000));
}

CAMLprim value getWindowWidth(value ocamlWindow) {
  CAMLparam1(ocamlWindow);
  JNIEnv* env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject glView = (jobject)(void *)Field(ocamlWindow, 1);

  jclass viewClass = (*env)->GetObjectClass(env, glView);
  jmethodID getWidth = (*env)->GetMethodID(env, viewClass, "getWidth", "()I");
  int width =  (*env)->CallIntMethod(env, glView, getWidth);
  LOGI("========= window width %d", width);

  CAMLreturn(Val_int(width));
  // CAMLreturn(Val_int(500));
}

CAMLprim value getDevicePixelRatio(value ocamlWindow) {
  CAMLparam1(ocamlWindow);
  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);
  jfieldID field = (*g_env)->GetFieldID(g_env, cls, "pixelDensity", "D");
  // jmethodID method = (*g_env)->GetMethodID(g_env, cls, "getDevicePixelRatio", "()D");
  // double pixelRatio = (*g_env)->CallDoubleMethod(g_env, g_pngmgr, method);
  double pixelRatio = (*g_env)->GetDoubleField(g_env, g_pngmgr, field);

  LOGI("========= pixel density %f", pixelRatio);
  CAMLreturn(caml_copy_double(pixelRatio));
}

#define Val_none Val_int(0)

static value Val_some(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc_small(1, 0);
  Field(some, 0) = v;
  CAMLreturn(some);
}

void texImage2DWithBitmap(value ocamlWindow, value target, value level, value bitmap, value border) {
  CAMLparam5(ocamlWindow, target, level, bitmap, border);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "texImage2DWithBitmap", "(IILandroid/graphics/Bitmap;I)V");
  (*g_env)->CallVoidMethod(g_env, g_pngmgr, method, Int_val(target), Int_val(level), (jobject)(void*)bitmap, Int_val(border));
  CAMLreturn0;
}

void fillTextureWithColor_native(value ocamlWindow, value target, value level, value red, value green, value blue, value alpha) {
  CAMLparam5(ocamlWindow, target, level, red, green);
  CAMLxparam2(blue, alpha);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "fillTextureWithColor", "(IIIIII)V");
  (*g_env)->CallVoidMethod(g_env, g_pngmgr, method, Int_val(target), Int_val(level), Int_val(red), Int_val(green), Int_val(blue), Int_val(alpha));

  CAMLreturn0;
}

void fillTextureWithColor_bytecode(value * argv, int argn) {
  fillTextureWithColor_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
}

CAMLprim value loadImage(value ocamlWindow, value filename) {
  CAMLparam2(ocamlWindow, filename);
  CAMLlocal1(record_image_data);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  /* Ask the PNG manager for a bitmap */
  jmethodID openBitmap = (*g_env)->GetMethodID(g_env, cls, "openBitmap",
                          "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
  jstring name = (*g_env)->NewStringUTF(g_env, String_val(filename));
  jobject png = (*g_env)->CallObjectMethod(g_env, g_pngmgr, openBitmap, name);
  (*g_env)->DeleteLocalRef(g_env, name);

  if (!png) {
    CAMLreturn(Val_none);
  } else {
    /* Get image dimensions */
    jmethodID getBitmapWidth = (*g_env)->GetMethodID(g_env, cls, "getBitmapWidth", "(Landroid/graphics/Bitmap;)I");
    int width = (*g_env)->CallIntMethod(g_env, g_pngmgr, getBitmapWidth, png);
    jmethodID getBitmapHeight = (*g_env)->GetMethodID(g_env, cls, "getBitmapHeight", "(Landroid/graphics/Bitmap;)I");
    int height = (*g_env)->CallIntMethod(g_env, g_pngmgr, getBitmapHeight, png);

    record_image_data = caml_alloc_small(3, 0);
    Field(record_image_data, 0) = Val_int(width);
    Field(record_image_data, 1) = Val_int(height);
    Field(record_image_data, 2) = (long)png;
    CAMLreturn(Val_some(record_image_data));
  }
}

CAMLprim value loadFile(value ocamlWindow, value filename) {
  CAMLparam2(ocamlWindow, filename);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "readFileContents",
                          "(Ljava/lang/String;)Ljava/lang/String;");
  jstring name = (*g_env)->NewStringUTF(g_env, String_val(filename));
  jstring contents = (*g_env)->CallObjectMethod(g_env, g_pngmgr, method, name);
  (*g_env)->DeleteLocalRef(g_env, name);

  if (contents) {
    const char *nativeString = (*g_env)->GetStringUTFChars(g_env, contents, 0);
    value result = Val_some(caml_copy_string(nativeString));
    CAMLreturn(result);
  } else {
    CAMLreturn(Val_none);
  }
}

CAMLprim void saveData(value ocamlWindow, value key, value data) {
  CAMLparam3(ocamlWindow, key, data);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "saveUserData",
                          "(Ljava/lang/String;[B)V");
  jstring name = (*g_env)->NewStringUTF(g_env, String_val(key));

  int size = caml_string_length(data);
  jbyteArray array = (*g_env)->NewByteArray(g_env, size);
  (*g_env)->SetByteArrayRegion(g_env, array, 0, size, (jbyte*)String_val(data));

  (*g_env)->CallVoidMethod(g_env, g_pngmgr, method, name, array);
  (*g_env)->DeleteLocalRef(g_env, name);
  CAMLreturn0;
}

CAMLprim value loadData(value ocamlWindow, value key) {
  CAMLparam2(ocamlWindow, key);
  CAMLlocal1(ml_data);

  JNIEnv* g_env = (JNIEnv*)(void *)Field(ocamlWindow, 0);
  jobject g_pngmgr = (jobject)(void *)Field(ocamlWindow, 2);

  jclass cls = (*g_env)->GetObjectClass(g_env, g_pngmgr);

  jmethodID method = (*g_env)->GetMethodID(g_env, cls, "loadUserData",
                          "(Ljava/lang/String;)[B");

  jstring name = (*g_env)->NewStringUTF(g_env, String_val(key));
  jbyteArray array = (*g_env)->CallObjectMethod(g_env, g_pngmgr, method, name);
  (*g_env)->DeleteLocalRef(g_env, name);

  if (!array) {
    CAMLreturn(Val_none);
  } else {
    int len = (*g_env)->GetArrayLength(g_env, array);
    char buf[len];
    (*g_env)->GetByteArrayRegion(g_env, array, 0, len, (jbyte*)buf);

    // from https://www.linux-nantes.org/~fmonnier/OCaml/ocaml-wrapping-c.html
    ml_data = caml_alloc_string(len);
    memcpy( String_val(ml_data), buf, len );

    CAMLreturn(Val_some(ml_data));
  }
}