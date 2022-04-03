#include "AndroidClient.h"

#ifdef HTTPS_BACKEND_ANDROID

#include <sstream>
#include <type_traits>

#include <dlfcn.h>

static std::string replace(const std::string &str, const std::string &from, const std::string &to)
{
	std::stringstream ss;
	size_t oldpos = 0;

	while (true)
	{
		size_t pos = str.find(from, oldpos);

		if (pos == std::string::npos)
		{
			ss << str.substr(oldpos);
			break;
		}

		ss << str.substr(oldpos, pos - oldpos) << to;
		oldpos = pos + from.length();
	}

	return ss.str();
}

static jstring newStringUTF(JNIEnv *env, const std::string &str)
{
	// We want std::string that contains null byte, hence length of 1.
	static std::string null("", 1);

	std::string newStr = replace(str, null, "\xC0\x80");
	jstring jstr = env->NewStringUTF(newStr.c_str());
	return jstr;
}

static std::string getStringUTF(JNIEnv *env, jstring str)
{
	// We want std::string that contains null byte, hence length of 1.
	static std::string null("", 1);

	const char *c = env->GetStringUTFChars(str, nullptr);
	std::string result = replace(c, "\xC0\x80", null);

	env->ReleaseStringUTFChars(str, c);
	return result;
}

AndroidClient::AndroidClient()
: HTTPSClient()
, SDL_AndroidGetJNIEnv(nullptr)
{
	// Look for SDL_AndroidGetJNIEnv
	SDL_AndroidGetJNIEnv = (decltype(SDL_AndroidGetJNIEnv)) dlsym(RTLD_DEFAULT, "SDL_AndroidGetJNIEnv");
	// Look for SDL_AndroidGetActivity
	SDL_AndroidGetActivity = (decltype(SDL_AndroidGetActivity)) dlsym(RTLD_DEFAULT, "SDL_AndroidGetActivity");
}

bool AndroidClient::valid() const
{
	if (SDL_AndroidGetJNIEnv && SDL_AndroidGetActivity)
	{
		JNIEnv *env = SDL_AndroidGetJNIEnv();

		if (env)
		{
			jclass httpsClass = getHTTPSClass();
			if (env->ExceptionCheck())
			{
				env->ExceptionClear();
				return false;
			}

			env->DeleteLocalRef(httpsClass);
			return true;
		}
	}

	return false;
}

HTTPSClient::Reply AndroidClient::request(const HTTPSClient::Request &req)
{
	JNIEnv *env = SDL_AndroidGetJNIEnv();
	jclass httpsClass = getHTTPSClass();

	if (httpsClass == nullptr)
	{
		env->ExceptionClear();
		throw std::runtime_error("Could not find class 'org.love2d.luahttps.LuaHTTPS'");
	}

	jmethodID constructor = env->GetMethodID(httpsClass, "<init>", "()V");
	jmethodID setURL = env->GetMethodID(httpsClass, "setUrl", "(Ljava/lang/String;)V");
	jmethodID request = env->GetMethodID(httpsClass, "request", "()Z");
	jmethodID getInterleavedHeaders = env->GetMethodID(httpsClass, "getInterleavedHeaders", "()[Ljava/lang/String;");
	jmethodID getResponse = env->GetMethodID(httpsClass, "getResponse", "()[B");
	jmethodID getResponseCode = env->GetMethodID(httpsClass, "getResponseCode", "()I");

	jobject httpsObject = env->NewObject(httpsClass, constructor);

	// Set URL
	jstring url = env->NewStringUTF(req.url.c_str());
	env->CallVoidMethod(httpsObject, setURL, url);
	env->DeleteLocalRef(url);

	// Set post data
	if (req.method == Request::POST)
	{
		jmethodID setPostData = env->GetMethodID(httpsClass, "setPostData", "([B)V");
		jbyteArray byteArray = env->NewByteArray((jsize) req.postdata.length());
		jbyte *byteArrayData = env->GetByteArrayElements(byteArray, nullptr);

		memcpy(byteArrayData, req.postdata.data(), req.postdata.length());
		env->ReleaseByteArrayElements(byteArray, byteArrayData, 0);

		env->CallVoidMethod(httpsObject, setPostData, byteArray);
		env->DeleteLocalRef(byteArray);
	}

	// Set headers
	if (!req.headers.empty())
	{
		jmethodID addHeader = env->GetMethodID(httpsClass, "addHeader", "(Ljava/lang/String;Ljava/lang/String;)V");

		for (auto &header : req.headers)
		{
			jstring headerKey = newStringUTF(env, header.first);
			jstring headerValue = newStringUTF(env, header.second);

			env->CallVoidMethod(httpsObject, addHeader, headerKey, headerValue);
			env->DeleteLocalRef(headerKey);
			env->DeleteLocalRef(headerValue);
		}
	}

	// Do request
	HTTPSClient::Reply response;
	jboolean status = env->CallBooleanMethod(httpsObject, request);

	// Get response
	response.responseCode = env->CallIntMethod(httpsObject, getResponseCode);

	if (status)
	{
		// Get headers
		jobjectArray interleavedHeaders = (jobjectArray) env->CallObjectMethod(httpsObject, getInterleavedHeaders);
		int len = env->GetArrayLength(interleavedHeaders);

		for (int i = 0; i < len; i += 2)
		{
			jstring key = (jstring) env->GetObjectArrayElement(interleavedHeaders, i);
			jstring value = (jstring) env->GetObjectArrayElement(interleavedHeaders, i + 1);

			response.headers[getStringUTF(env, key)] = getStringUTF(env, value);

			env->DeleteLocalRef(key);
			env->DeleteLocalRef(value);
		}

		env->DeleteLocalRef(interleavedHeaders);

		// Get response data
		jbyteArray responseData = (jbyteArray) env->CallObjectMethod(httpsObject, getResponse);

		if (responseData)
		{
			int len = env->GetArrayLength(responseData);
			jbyte *responseByte = env->GetByteArrayElements(responseData, nullptr);

			response.body = std::string((char *) responseByte, len);

			env->DeleteLocalRef(responseData);
		}
	}

	return response;
}

jclass AndroidClient::getHTTPSClass() const
{
	JNIEnv *env = SDL_AndroidGetJNIEnv();

	jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
	jmethodID loadClass = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

	jobject activity = SDL_AndroidGetActivity();

	if (activity == nullptr)
		return nullptr;

	jclass gameActivity = env->GetObjectClass(activity);
	jmethodID getLoader = env->GetMethodID(gameActivity, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject classLoader = env->CallObjectMethod(activity, getLoader);

	jstring httpsClassName = env->NewStringUTF("org.love2d.luahttps.LuaHTTPS");
	jclass httpsClass = (jclass) env->CallObjectMethod(classLoader, loadClass, httpsClassName);

	env->DeleteLocalRef(gameActivity);
	env->DeleteLocalRef(httpsClassName);
	env->DeleteLocalRef(activity);
	env->DeleteLocalRef(classLoaderClass);

	return httpsClass;
}

#endif // HTTPS_BACKEND_ANDROID