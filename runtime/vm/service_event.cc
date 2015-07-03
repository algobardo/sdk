// Copyright (c) 2015, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/service_event.h"

namespace dart {

// Translate from the legacy DebugEvent to a ServiceEvent.
static ServiceEvent::EventType TranslateEventType(
    DebuggerEvent::EventType type) {
    switch (type) {
      case DebuggerEvent::kIsolateCreated:
        return ServiceEvent::kIsolateStart;

      case DebuggerEvent::kIsolateShutdown:
        return ServiceEvent::kIsolateExit;

      case DebuggerEvent::kBreakpointReached:
        return ServiceEvent::kPauseBreakpoint;

      case DebuggerEvent::kIsolateInterrupted:
        return ServiceEvent::kPauseInterrupted;

      case DebuggerEvent::kExceptionThrown:
        return ServiceEvent::kPauseException;

      default:
        UNREACHABLE();
        return ServiceEvent::kIllegal;
    }
}

ServiceEvent::ServiceEvent(const DebuggerEvent* debugger_event)
    : isolate_(debugger_event->isolate()),
      type_(TranslateEventType(debugger_event->type())),
      breakpoint_(NULL),
      top_frame_(NULL),
      exception_(NULL),
      inspectee_(NULL),
      gc_stats_(NULL) {
  DebuggerEvent::EventType type = debugger_event->type();
  if (type == DebuggerEvent::kBreakpointReached) {
    set_breakpoint(debugger_event->breakpoint());
  }
  if (type == DebuggerEvent::kExceptionThrown) {
    set_exception(debugger_event->exception());
  }
  if (type == DebuggerEvent::kBreakpointReached ||
      type == DebuggerEvent::kIsolateInterrupted ||
      type == DebuggerEvent::kExceptionThrown) {
    set_top_frame(debugger_event->top_frame());
  }
}


const char* ServiceEvent::EventTypeToCString(EventType type) {
  switch (type) {
    case kIsolateStart:
      return "IsolateStart";
    case kIsolateExit:
      return "IsolateExit";
    case kIsolateUpdate:
      return "IsolateUpdate";
    case kPauseStart:
      return "PauseStart";
    case kPauseExit:
      return "PauseExit";
    case kPauseBreakpoint:
      return "PauseBreakpoint";
    case kPauseInterrupted:
      return "PauseInterrupted";
    case kPauseException:
      return "PauseException";
    case kResume:
      return "Resume";
    case kBreakpointAdded:
      return "BreakpointAdded";
    case kBreakpointResolved:
      return "BreakpointResolved";
    case kBreakpointRemoved:
      return "BreakpointRemoved";
    case kGC:
      return "GC";  // TODO(koda): Change to GarbageCollected.
    case kInspect:
      return "Inspect";
    case kDebuggerSettingsUpdate:
      return "_DebuggerSettingsUpdate";
    case kIllegal:
      return "Illegal";
    default:
      UNREACHABLE();
      return "Unknown";
  }
}


const char* ServiceEvent::stream_id() const {
  switch (type()) {
    case kIsolateStart:
    case kIsolateExit:
    case kIsolateUpdate:
      return "Isolate";

    case kPauseStart:
    case kPauseExit:
    case kPauseBreakpoint:
    case kPauseInterrupted:
    case kPauseException:
    case kResume:
    case kBreakpointAdded:
    case kBreakpointResolved:
    case kBreakpointRemoved:
    case kInspect:
    case kDebuggerSettingsUpdate:
      return "Debug";

    case kGC:
      return "GC";

    default:
      UNREACHABLE();
      return NULL;
  }
}


void ServiceEvent::PrintJSON(JSONStream* js) const {
  JSONObject jsobj(js);
  jsobj.AddProperty("type", "Event");
  jsobj.AddProperty("kind", EventTypeToCString(type()));
  jsobj.AddProperty("isolate", isolate());
  if (type() == kPauseBreakpoint) {
    JSONArray jsarr(&jsobj, "pauseBreakpoints");
    // TODO(rmacnak): If we are paused at more than one breakpoint,
    // provide it here.
    if (breakpoint() != NULL) {
      jsarr.AddValue(breakpoint());
    }
  } else {
    if (breakpoint() != NULL) {
      jsobj.AddProperty("breakpoint", breakpoint());
    }
  }
  if (type() == kDebuggerSettingsUpdate) {
    JSONObject jssettings(&jsobj, "_debuggerSettings");
    isolate()->debugger()->PrintSettingsToJSONObject(&jssettings);
  }
  if (top_frame() != NULL) {
    JSONObject jsFrame(&jsobj, "topFrame");
    top_frame()->PrintToJSONObject(&jsFrame);
    intptr_t index = 0;  // Avoid ambiguity in call to AddProperty.
    jsFrame.AddProperty("index", index);
  }
  if (exception() != NULL) {
    jsobj.AddProperty("exception", *(exception()));
  }
  if (inspectee() != NULL) {
    jsobj.AddProperty("inspectee", *(inspectee()));
  }
  if (gc_stats() != NULL) {
    jsobj.AddProperty("reason", Heap::GCReasonToString(gc_stats()->reason_));
    isolate()->heap()->PrintToJSONObject(Heap::kNew, &jsobj);
    isolate()->heap()->PrintToJSONObject(Heap::kOld, &jsobj);
  }
}

}  // namespace dart
