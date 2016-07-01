/*
 * src/sensor.cc
 * Copyright(c) 2016 Ryan Coffman
 * MIT Licensed
 */

#include <IOKit/IOKitLib.h>
#include <algorithm>
#include <iostream>
#include <nan.h>
#include <node.h>
#include <stdlib.h>

using node::AtExit;
using std::max;
using std::min;
using v8::String;
using v8::FunctionTemplate;
using v8::Local;
using v8::Value;
using v8::Number;
using v8::Function;
using Nan::New;
using Nan::GetFunction;
using Nan::Callback;
using Nan::AsyncWorker;
using Nan::AsyncQueueWorker;
using Nan::Null;
using Nan::Set;

// Code for accessing sensor taken from here:
// https://code.google.com/archive/p/google-mac-qtz-patches/
io_connect_t dataPort = IO_OBJECT_NULL;

enum {
  kGetSensorReadingID = 0, // getSensorReading(int *, int *)
  kGetLEDBrightnessID = 1, // getLEDBrightness(int, int *)
  kSetLEDBrightnessID = 2, // setLEDBrightness(int, int, int *)
  kSetLEDFadeID = 3,       // setLEDFade(int, int, int, int *)
};

class ALSWorker : public AsyncWorker {
public:
  uint64_t value;
  kern_return_t status;

  ALSWorker(Callback *callback) : AsyncWorker(callback) {}

  void Execute() {
    uint32_t outputCount = 2;
    uint64_t output[outputCount];
    status = IOConnectCallMethod(dataPort, kGetSensorReadingID, NULL, 0, NULL,
                                 0, output, &outputCount, NULL, 0);
    value = max(output[0], output[1]);
  }

  void HandleOKCallback() {
    // Max value appears to be 67,092,480 on the esarly 2015 MacBook Pros
    Local<Value> argv[]{Null(), New<Number>(min((double)value / 67092480, 1.0)),
                        New<Number>(value)};
    callback->Call(3, argv);
  }
};
NAN_METHOD(ReadALS) {
  Callback *callback = new Callback(info[0].As<Function>());
  AsyncQueueWorker(new ALSWorker(callback));
}

static void onExit(void *arg) {
  io_service_t service = *static_cast<io_service_t *>(arg);
  if (service) {
    IOServiceClose(service);
    dataPort = IO_OBJECT_NULL;
  }
}

NAN_MODULE_INIT(Init) {
  io_service_t service = IOServiceGetMatchingService(
      kIOMasterPortDefault, IOServiceMatching("AppleLMUController"));
  kern_return_t status = KERN_FAILURE;
  if (service) {
    status = IOServiceOpen(service, mach_task_self(), 0, &dataPort);
    IOObjectRelease(service);
  }

  Set(target, New<String>("readALS").ToLocalChecked(),
      GetFunction(New<FunctionTemplate>(ReadALS)).ToLocalChecked());

  AtExit(onExit, &service);
}

NODE_MODULE(sensor, Init)
