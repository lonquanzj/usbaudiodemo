#include "ToJava.h"
#include "PortDefs.h"
#include <wx/string.h>
#include <signal.h>
#include <stdarg.h>
#include "../libusb/libusb.h"

static JavaVM *gJavaVM = NULL;
static bool isAttached = false;
static wxString s_logFileName;
static int s_logMethod = 0;
static jclass s_progressClass = NULL;


wxString s_packageName = "";


void getGlobalRef(JNIEnv* i_env, const char* clazz, jclass* globalClass)
{
    JNIEnv* env = GetEnv();

    jclass local = i_env->FindClass(clazz);
    if (env->ExceptionOccurred()) // FindClass does not return NULL but throws an exception, clear it
    {
        env->ExceptionClear();
        return;
    } 
    
    if (local)
    {
      *globalClass = (jclass) env->NewGlobalRef(local);
      env->DeleteLocalRef(local);
    }
}


static struct sigaction old_sa[NSIG];

void android_sigaction(int signal, siginfo_t *info, void *reserved)
{
    wxLogErrorMain("Crashed in native code! Signal = %d, code = %d, errno = %d, signo = %d", signal, info->si_code, info->si_errno, info->si_signo);

    JNIEnv* env = GetEnv();
    if (env)
    {
        wxString ProgressString = s_packageName + "/Progress";
        jclass progressClass = env->FindClass(ProgressString.c_str());

        if (progressClass != 0)
        {
            // Call the Java static method Progress.nativeCrashed()
            jmethodID method = env->GetStaticMethodID(progressClass, "nativeCrashed", "()V");
            
            if (method != 0)
            {
                env->CallStaticVoidMethod(progressClass, method);
            }
            else
            {
                logIt("Did NOT find nativeCrashed!");
            }

            env->DeleteLocalRef(progressClass);
        }
    }

    old_sa[signal].sa_handler(signal);
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    gJavaVM = vm;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        logIt("Failed to get the environment using GetEnv()");
        return -1;
    }

    // set up a native signal handler to catch native exceptions/crashes
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_sigaction = android_sigaction;
    handler.sa_flags = SA_RESETHAND;
#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
    CATCHSIG(SIGILL);
    CATCHSIG(SIGABRT);
    CATCHSIG(SIGBUS);
    CATCHSIG(SIGFPE);
    CATCHSIG(SIGSEGV);
    CATCHSIG(SIGSTKFLT);
    CATCHSIG(SIGPIPE);

    return JNI_VERSION_1_4;
}


void removeSignalHandler()
{
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_sigaction = android_sigaction;
    handler.sa_flags = SA_RESETHAND;
#define CATCHSIG2(X) sigaction(X, &old_sa[X], NULL)
    CATCHSIG2(SIGILL);
    CATCHSIG2(SIGABRT);
    CATCHSIG2(SIGBUS);
    CATCHSIG2(SIGFPE);
    CATCHSIG2(SIGSEGV);
    CATCHSIG2(SIGSTKFLT);
    CATCHSIG2(SIGPIPE);
}


JavaVM *getJavaVM()
{
    return gJavaVM;
}


JNIEnv* GetEnv()
{
    int status;
    JNIEnv *env;

    status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);

    if (status < 0)
    {
        wxLogDebugMain("callback_handler: failed to get JNI environment, assuming native thread");
        status = gJavaVM->AttachCurrentThread(&env, NULL);
        if (status < 0)
        {
            logIt("callback_handler: failed to attach current thread");
            return NULL;
        }

        isAttached = true;
    }

    return env;
}

void DoMessage(const wxString &i_Message)
{
	jmethodID method;
    JNIEnv* env = GetEnv();

    if (env)
    {
        wxString ProgressString = s_packageName + "/Progress";
        jclass progressClass = env->FindClass(ProgressString.c_str());

        if (progressClass != 0)
        {
            // Call the Java static method Progress.showMessage(String i_message)
            method = env->GetStaticMethodID(progressClass, "showMessage", "(Ljava/lang/String;)V");
            
            if (method != 0)
            {
                jstring str = env->NewStringUTF(i_Message.c_str());  
                env->CallStaticVoidMethod(progressClass, method, str);
                env->DeleteLocalRef(str);
            }
            else
            {
                logIt("Did NOT find showMessage!");
            }

            env->DeleteLocalRef(progressClass);
        }
    }
    else
    {
        logIt("env is NULL!");
    }
}


void AppendLog(const wxString &i_Message)
{
	jmethodID method;
    JNIEnv* env = GetEnv();

    if (env)
    {
        wxString ProgressString = s_packageName + "/Progress";
        jclass progressClass = env->FindClass(ProgressString.c_str());

        if (progressClass != 0)
        {
            // Call the Java static method Progress.logMessage(String i_message)
            method = env->GetStaticMethodID(progressClass, "logMessage", "(Ljava/lang/String;)V");
            
            if (method != 0)
            {
                jstring str = env->NewStringUTF(i_Message.c_str());  
                env->CallStaticVoidMethod(progressClass, method, str);
                env->DeleteLocalRef(str);
            }
            else
            {
                logIt("Did NOT find logMessage!");
            }

            env->DeleteLocalRef(progressClass);
        }
    }
    else
    {
        logIt("env is NULL!");
    }
}


int getLogMethod()
{
    return s_logMethod;
}


void setLogMethod(int i_methodNr)
{
    if (i_methodNr >=0 && i_methodNr <= 2)
    {
        s_logMethod = i_methodNr;
        libusb_setLogMethod(i_methodNr);
    }
}



void setLogFileName(wxString i_logFileName)
{
    s_logFileName = i_logFileName;
    libusb_setLogFileName(i_logFileName.c_str());
}


void logIt(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    if (s_logMethod == 2)
    {
        __android_log_vprint(ANDROID_LOG_DEBUG, "Main", fmt, argp);
    }
    else if (s_logMethod == 1)
    {
        extern wxString s_logFileName;

        wxLogDebugMain("WAT???");
        FILE *logFile = fopen(s_logFileName.c_str(), "a");
        if (logFile)
        {
            vfprintf(logFile, fmt, argp);
            fprintf(logFile, "\r\n");
            fclose(logFile);
        }
        else
        {
            wxLogDebugMain("GEK!");
            __android_log_print(ANDROID_LOG_DEBUG, "Main", "couldn't open logfile!");
        }
    }

    va_end(argp);
}


void logError(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    __android_log_vprint(ANDROID_LOG_ERROR, "Main", fmt, argp);
    extern wxString s_logFileName;

    FILE *logFile = fopen(s_logFileName.c_str(), "a");
    if (logFile)
    {
        vfprintf(logFile, fmt, argp);
        fprintf(logFile, "\r\n");
        fclose(logFile);
    }

    va_end(argp);
}


void setPackageName(wxString i_PackageName)
{
    s_packageName = i_PackageName;

    wxString ProgressString = s_packageName + "/Progress";
    jclass progressClass = GetEnv()->FindClass(ProgressString.c_str());

     /* Create a global reference */
     s_progressClass = (jclass) GetEnv()->NewGlobalRef(progressClass);

     /* The local reference is no longer useful */
     GetEnv()->DeleteLocalRef(progressClass);
}


void DoStopJava(const wxString &i_Message)
{
	jmethodID method;
    JNIEnv* env = GetEnv();

    if (env)
    {
        wxString ProgressString = s_packageName + "/Progress";

        if (s_progressClass != 0)
        {
            method = env->GetStaticMethodID(s_progressClass, "stopJava", "(Ljava/lang/String;)V");
            
            if (method != 0)
            {
                jstring str = env->NewStringUTF(i_Message.c_str());  
                env->CallStaticVoidMethod(s_progressClass, method, str);
                env->DeleteLocalRef(str);
            }
            else
            {
                logIt("Did NOT find stopJava!");
            }
        }
        else
        {
            wxLogErrorMain("Did not find progress class!");
        }
    }
    else
    {
        logIt("env is NULL!");
    }
}
