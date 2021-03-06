/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   16:41
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DoPixel.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DoPixel
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	DoPixel Interface
*********************************************************************/
#ifndef __DOPIXEL__
#define __DOPIXEL__

#include "DpRef.h"
#include "DpLog.h"
#include "DpMath.h"

#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace dopixel
{
	// macros
#define SAFE_DELETE(p)		do { if (p != nullptr) { delete p; p = nullptr; } } while(0)
#define SAFE_DELETEARRAY(p) do { if (p != nullptr) { delete[] p; p = nullptr; } } while(0)
#define SAFE_RELEASE(p)		do { if (p != nullptr) { p->Release(); p = nullptr; } } while(0)

#define ASSERT	assert
#define BIT(x)      (1<<(x))

#define DECLARE_ENUM(name) \
		namespace name { \
			enum Type {

#define END_DECLARE_ENUM() \
				,Max \
			}; \
		}

	// enum types
	DECLARE_ENUM(PixelFormat)
		Unknown,
		RGB,
		RGBA,
		ARGB,
		FLOAT4 //RGBA
	END_DECLARE_ENUM()

	DECLARE_ENUM(TextureFilter)
		NearestPoint,
		Bilinear,
		Trilinear
	END_DECLARE_ENUM()

	DECLARE_ENUM(TextureWrap)
		Reapeat,
		Mirror,
		ClampToEdge,
		ClampToBorder
	END_DECLARE_ENUM()

	DECLARE_ENUM(TextureUsage)
		Diffuse,
		Specular,
		Normal
	END_DECLARE_ENUM()

	DECLARE_ENUM(ColorUsage)
		Ambient,
		Diffuse,
		Specular,
		Emissive
	END_DECLARE_ENUM()

	DECLARE_ENUM(VertexType)
		Position = BIT(0),
		Normal = BIT(1),
		Color = BIT(2),
		TexCoord = BIT(3)
	END_DECLARE_ENUM()

	DECLARE_ENUM(PrimitiveType)
		Points,
		Lines,
		Triangles
	END_DECLARE_ENUM()

	DECLARE_ENUM(ShadeMode)
		Wireframe,
		Constant,
		Flat,
		Gouraud,
		Phong
	END_DECLARE_ENUM()

	DECLARE_ENUM(CullMode)
		None,
		Front,
		Back
	END_DECLARE_ENUM()

	DECLARE_ENUM(ZBuffer)
		None,
		ZBuff,
		INVZBuff
	END_DECLARE_ENUM()

	DECLARE_ENUM(Condition)
		LessThan,
		LessThanEqual,
		Equal,
		NotEqual,
		GreaterThanEqual,
		GreaterThan
	END_DECLARE_ENUM()

	DECLARE_ENUM(Transform)
		World,
		View,
		Projection
	END_DECLARE_ENUM()

	enum { MAX_LIGHTS = 8 };

	using std::string;
	using std::wstring;
	using std::vector;
	using std::map;
	using std::pair;

	class VertexBuffer;
	class IndexBuffer;
	class Image;
	class Texture;
	class Material;
	class Mesh;
	class SubMesh;
	class Loader;
	class SceneNode;
	class CameraSceneNode;
	class SceneNodeAnimator;
	class SceneManager;
	class Camera;
	class CameraController;
	class Light;
	class Renderer;
	class Rasterizer;
	class Event;
	class MouseMoveEvent;
	class MousePressEvent;
	class MouseReleaseEvent;
	class MouseWheelEvent;
	class KeyPressEvent;
	class KeyReleaseEvent;

	typedef Ref<VertexBuffer> VertexBufferRef;
	typedef Ref<IndexBuffer> IndexBufferRef;
	typedef Ref<Image> ImageRef;
	typedef Ref<Texture> TextureRef;
	typedef Ref<Material> MaterialRef;
	typedef Ref<Mesh> MeshRef;
	typedef Ref<SubMesh> SubMeshRef;
	typedef Ref<Loader> LoaderRef;
	typedef Ref<SceneNode> SceneNodeRef;
	typedef Ref<CameraSceneNode> CameraSceneNodeRef;
	typedef Ref<SceneNodeAnimator> SceneNodeAnimatorRef;
	typedef Ref<SceneManager> SceneManagerRef;
	typedef Ref<Camera> CameraRef;
	typedef Ref<CameraController> CameraControllerRef;
	typedef Ref<Event> EventRef;
	typedef Ref<Light> LightRef;
	typedef Ref<Renderer> RendererRef;
	typedef Ref<Rasterizer> RasterizerRef;

	// can not copy
	class NoCopyable
	{
	public:
		NoCopyable() {}
		NoCopyable(const NoCopyable&) = delete;
		NoCopyable& operator=(const NoCopyable&) = delete;
	};

	// scope guard
	class ScopeGuard : public NoCopyable
	{
	public:
		ScopeGuard(std::function<void()> func) : func_(func) {}
		~ScopeGuard() { func_(); }
	private:
		std::function<void()> func_;
	};
	#define SCOPEGUARD_NAMELINE_CAT(name, line) name##line
	#define SCOPEGUARD_NAMELINE(name, line) SCOPEGUARD_NAMELINE_CAT(name, line)
	#define ON_SCOPE_EXIT(func) ScopeGuard SCOPEGUARD_NAMELINE(EXIT, __LINE__)(func)

	// time step
	class Timestep
	{
	public:
		explicit Timestep(int milliseconds) : milliseconds_(milliseconds) {}

		int GetMilliseconds() const { return milliseconds_; }
		float GetSeconds() const { return milliseconds_ * 0.001f; }
	private:
		int milliseconds_;
	};

	// swap
	template<typename T>
	inline void swap_t(T& a, T& b) { T t = a; a = b; b = t; }
	// equal
	template<typename T>
	inline bool equal_t(const T& a, const T& b) { return a == b; }
	inline bool equal_t(const float& a, const float& b) { return math::Equal(a, b); }

	// string utils

	// string copy
	void strncpy_s(char* pDest, const char* pSrc, int destSize);
	// string format
	// use internal static space hold format result, length limits to 1024
	const char* str_format(const char* format, ...);
	// string trim
	enum { TRIM_LEFT = 1, TRIM_RIGHT };
	void str_trim(string& str, int flag = TRIM_LEFT | TRIM_RIGHT);
	// string strip
	void str_strip(char* strDest, const char* strSrc, const char* strPattern);
	void str_strip(string& strDest, const string& strSrc, const string& strPattern);
	// string strip any char
	// common used in parse file, e.g. "<1,2> <2,3>" -> "1,2 2,3", where any char is:"<>" 
	void str_strip_anychar(char* strDest, const char* strSrc, const char* anychar);
	void str_strip_anychar(string& strDest, const string& strSrc, const string& anychar);
	// string split
	void str_split(vector<string>& vec, const string& str, char ch);
	// string replace
	void str_replace(string& strDest, const string& strSrc, const string& strPattern, const string& strReplace);
	// string replace by regex pattern
	void str_regex_replace(string& strDest, const string& strSrc, const string& strRegexPattern, const string& strReplace);
	// string replace any char
	void str_replace_anychar(string& strDest, const string& strSrc, const string& anychar, const string& strReplace);
	// string is valid float format
	bool str_is_valid_float(const string& str);

	// utils
	inline int GetVertexNumByPerPrimitive(int primitive)
	{
		int num = 0;
		switch (primitive)
		{
		case PrimitiveType::Triangles:
			num = 3;
			break;
		case PrimitiveType::Lines:
			num = 2;
			break;
		case PrimitiveType::Points:
			num = 1;
			break;
		default:
			ASSERT(0);
			break;
		}
		return num;
	}
}

#endif // !__DOPIXEL__
