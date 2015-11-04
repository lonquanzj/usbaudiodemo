#pragma once

#include <wx/string.h>

#include <jni.h>

struct AEException;

/// WARNING: jint JNI_OnLoad(JavaVM* vm, void* reserved) is present in this file. If you already have
/// a JNI_OnLoad(), make sure to replace this one in a function and call it from your JNI_Onload()!

/// Pass in your package name, like com.yourcompany.something.
/// You need to set this before you can call other functions like DoMessage and AppendLog because
/// they call back to the Java side.
/// Also attempts to find the Java class called 'Progress' in the com.yourcompany.something namespace.
/// If you want to use these functions, please rename 'Progress' to something sensible and implement
/// the Java methods that are called by these functions (see the code).
void setPackageName(wxString i_PackageName);


/// Supposed to do a Toast in Java
void DoMessage(const wxString &i_Message);

/// Calls Progress.logMessage(String i_message)
void AppendLog(const wxString &i_Message);

/// logs to the Android debug stream
void logIt(const char *fmt, ...);

/// logs to the Android error stream
void logError(const char *fmt, ...);

/// 0 = no logging, 1 = log to file, 2 = log to debug stream
void setLogMethod(int i_methodNr);
int getLogMethod();

/// if log method is 1, uses this file name to append the log to
void setLogFileName(wxString i_logFileName);

/// Returns the Java Virtual Machine pointer, useful to call DetachCurrentThread()
JavaVM *getJavaVM();
JNIEnv* GetEnv();


/// JNI_OnLoad() sets up a signal handler to catch (crash) signals. This function removes the signal handler again.
void removeSignalHandler();


/// this can be called from within the USB thread, asking the USB playback to be stopped
void DoStopJava(const wxString &i_Message);