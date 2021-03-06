// (c) 2014 Boris van Schooten
//BEGIN_INCLUDE(all)
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <thread>
#include <functional>

#include <v8.h>
#include <glfw-bindings.h>
#include <platform/macos/include/ContextQuartz2D.h>
#include <bindings.h>
#include <openvr-bindings.h>

using namespace v8;

namespace exokit {

// NOTE: must already be in context
void callFunction(const char *funcname, const int argc, Local<Value> argv[]) {
  // init
  Isolate *isolate = Isolate::GetCurrent();
  Local<Context> localContext = isolate->GetCurrentContext();
  Local<Object> global = localContext->Global();

  // get function
  Local<String> jsfunc_name = String::NewFromUtf8(isolate,funcname);
  Local<Value> jsfunc_val = global->Get(jsfunc_name);
  if (!jsfunc_val->IsFunction()) return;
  Local<Function> jsfunc = Local<Function>::Cast(jsfunc_val);

  // call function, 'this' points to global object
  TryCatch try_catch;
  Local<Value> result = jsfunc->Call(global, argc, argv);

  if (result.IsEmpty()) {
    String::Utf8Value error(try_catch.Exception());
    String::Utf8Value stacktrace(try_catch.StackTrace());
    // LOGI("Error calling %s: %s:\n%s",funcname,*error,*stacktrace);
  } else {
    // LOGI("%s called",funcname);
  }
}

void Java_com_mafintosh_nodeonandroid_NodeService_onResize
() {
// (JNIEnv *env, jclass clas, jint width, jint height) {
	// LOGI("JNI onResize %d %d", width, height);

  {
    HandleScope handle_scope(Isolate::GetCurrent());
    
    unsigned int width = 1;
    unsigned int height = 1;

    Handle<Number> js_width = v8::Integer::New(Isolate::GetCurrent(), width);
    Handle<Number> js_height = v8::Integer::New(Isolate::GetCurrent(), height);

    Local<Value> argv[] = {js_width, js_height};
    callFunction("onResize", sizeof(argv)/sizeof(argv[0]), argv);
  }
}


void Java_com_mafintosh_nodeonandroid_NodeService_onNewFrame
() {
// (JNIEnv *env, jclass clas, jfloatArray headViewMatrix, jfloatArray headQuaternion, jfloatArray centerArray) {
  float headViewMatrixElements[] = {0};
  float headQuaternionElements[] = {0};
  float centerArrayElements[] = {0};

  {
    HandleScope handle_scope(Isolate::GetCurrent());

    Local<Float32Array> headMatrixFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 16 * 4), 0, 16);
    for (int i = 0; i < 16; i++) {
      headMatrixFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), headViewMatrixElements[i]));
    }
    Local<Float32Array> headQuaternionFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 4 * 4), 0, 4);
    for (int i = 0; i < 4; i++) {
      headQuaternionFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), headQuaternionElements[i]));
    }
    Local<Float32Array> centerFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 3 * 4), 0, 3);
    for (int i = 0; i < 3; i++) {
      centerFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), centerArrayElements[i]));
    }
    Local<Value> argv[] = {headMatrixFloat32Array, headQuaternionFloat32Array, centerFloat32Array};
    callFunction("onNewFrame", sizeof(argv)/sizeof(argv[0]), argv);
  }
}


void Java_com_mafintosh_nodeonandroid_NodeService_onDrawEye
() {
// (JNIEnv *env, jclass clasj, jfloatArray eyeViewMatrix, jfloatArray eyePerspectiveMatrix) {
  float eyeViewMatrixElements[] = {0};
  float eyePerspectiveMatrixElements[] = {0};

  {
    HandleScope handle_scope(Isolate::GetCurrent());

    Local<Float32Array> eyeViewMatrixFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 16 * 4), 0, 16);
    for (int i = 0; i < 16; i++) {
      eyeViewMatrixFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), eyeViewMatrixElements[i]));
    }
    Local<Float32Array> eyePerspectiveMatrixFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 4 * 4), 0, 4);
    for (int i = 0; i < 4; i++) {
      eyePerspectiveMatrixFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), eyePerspectiveMatrixElements[i]));
    }
    Local<Value> argv[] = {eyeViewMatrixFloat32Array, eyePerspectiveMatrixFloat32Array};
    callFunction("onDrawEye", sizeof(argv)/sizeof(argv[0]), argv);
  }
}


void Java_com_mafintosh_nodeonandroid_NodeService_onDrawFrame
() {
// (JNIEnv *env, jclass clas, jfloatArray viewMatrix, jfloatArray projectionMatrix, jfloatArray centerArray) {
  float viewMatrixElements[] = {0};
  float projectionMatrixElements[] = {0};
  float centerArrayElements[] = {0};

  {
    HandleScope handle_scope(Isolate::GetCurrent());

    Local<Float32Array> viewFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 16 * 4), 0, 16);
    for (int i = 0; i < 16; i++) {
      viewFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), viewMatrixElements[i]));
    }
    Local<Float32Array> projectionFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 16 * 4), 0, 16);
    for (int i = 0; i < 16; i++) {
      projectionFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), projectionMatrixElements[i]));
    }
    Local<Float32Array> centerFloat32Array = Float32Array::New(ArrayBuffer::New(Isolate::GetCurrent(), 3 * 4), 0, 3);
    for (int i = 0; i < 3; i++) {
      centerFloat32Array->Set(i, Number::New(Isolate::GetCurrent(), centerArrayElements[i]));
    }
    Local<Value> argv[] = {viewFloat32Array, projectionFloat32Array, centerFloat32Array};
    callFunction("onDrawFrame", sizeof(argv)/sizeof(argv[0]), argv);
  }
}

void Init(Handle<Object> exports) {
  canvas::Quartz2DContextFactory *canvasContextFactory = new canvas::Quartz2DContextFactory(1);
  CanvasRenderingContext2D::InitalizeStatic(canvasContextFactory);
  
  canvas::ImageData::setFlip(true);

  Local<Value> gl = makeGl();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeGl"), gl);

  Local<Value> image = makeImage();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeImage"), image);

  Local<Value> imageData = makeImageData();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeImageData"), imageData);

  Local<Value> imageBitmap = makeImageBitmap();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeImageBitmap"), imageBitmap);

  Local<Value> canvas = makeCanvasRenderingContext2D(imageData);
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeCanvasRenderingContext2D"), canvas);

  Local<Value> path2d = makePath2D();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativePath2D"), path2d);
  
  /* Local<Value> glfw = makeGlfw();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeGlfw"), glfw); */
  
  Local<Value> window = makeWindow();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeWindow"), window);
  
  Local<Value> vr = makeVr();
  exports->Set(v8::String::NewFromUtf8(Isolate::GetCurrent(), "nativeVr"), vr);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}