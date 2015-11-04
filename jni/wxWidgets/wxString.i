%typemap(jtype) wxString "String"
%typemap(jstype) wxString "String"
%typemap(jni) wxString "jstring"
%typemap(javaout) wxString
{
    return new String($jnicall);
}


%typemap(out) wxString // C++ -> Java
{
    $result = jenv->NewStringUTF($1.ToAscii());
}


%typemap(javain) wxString "$javainput"


%typemap(in) wxString {
   jboolean iscopy;
   const char* c_string = jenv->GetStringUTFChars((jstring)$input, &iscopy); 
   $1 = wxString(c_string);
 }
