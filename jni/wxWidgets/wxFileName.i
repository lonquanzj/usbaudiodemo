%typemap(jtype) wxFileName "String"
%typemap(jstype) wxFileName "String"
%typemap(jni) wxFileName "jstring"
%typemap(javaout) wxFileName
{
    return new String($jnicall);
}


%typemap(out) wxFileName // C++ -> Java
{
    $result = jenv->NewStringUTF($1.GetFullPath().ToAscii());
}


%typemap(javain) wxFileName "$javainput"


%typemap(in) wxFileName {
   jboolean iscopy;
   const char* c_string = jenv->GetStringUTFChars((jstring)$input, &iscopy); 
   $1 = wxFileName(c_string);
 }
