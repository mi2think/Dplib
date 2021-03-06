/********************************************************************
	created:	2014/08/06
	created:	6:8:2014   22:06
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpLight.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpLight
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Light
*********************************************************************/

#ifndef __DP_LIGHT__
#define __DP_LIGHT__

#include "DoPixel.h"
#include "DpMath.h"
#include "DpVector3.h"

namespace dopixel
{
	class Light
	{
	public:
		Light(
			const math::Vector3f& ambient,
			const math::Vector3f& diffuse,
			const math::Vector3f& specular,
			float range = FLT_MAX);
		virtual ~Light() = 0;

		virtual void BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const = 0;
		virtual math::Vector3f Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const = 0;	
		virtual math::Vector3f IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const = 0;		
		virtual void EndLighting() const = 0;
	protected:
		math::Vector3f ambient_;
		math::Vector3f diffuse_;
		math::Vector3f specular_;
		float range_;
	};

	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(
			const math::Vector3f& lightWorldDir,
			const math::Vector3f& ambient,
			const math::Vector3f& diffuse,
			const math::Vector3f& specular,
			float range = FLT_MAX);
		~DirectionalLight();

		void BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const;
		math::Vector3f Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const;
		math::Vector3f IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const;
		void EndLighting() const;
	private:
		math::Vector3f lightWorldDir_;
		mutable math::Vector3f invLightWorldDir_;
		mutable math::Vector3f eyeWorldPos_;
		mutable const Material* material_;
	};

	class PointLight : public Light
	{
	public:
		PointLight(
			const math::Vector3f& lightWorldPos,
			const math::Vector3f& attenuation,
			const math::Vector3f& ambient,
			const math::Vector3f& diffuse,
			const math::Vector3f& specular,
			float range = FLT_MAX);
		~PointLight();

		void BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const;
		math::Vector3f Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const;
		math::Vector3f IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const;
		void EndLighting() const;
	private:
		math::Vector3f attenuation_;
		math::Vector3f lightWorldPos_;
		mutable math::Vector3f eyeWorldPos_;
		mutable const Material* material_;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight(
			const math::Vector3f& lightWorldDir,
			const math::Vector3f& lightWorldPos,
			const math::Vector3f& attenuation,
			float theta,
			float phi,
			float pf,
			const math::Vector3f& ambient,
			const math::Vector3f& diffuse,
			const math::Vector3f& specular,
			float range = FLT_MAX);
		~SpotLight();

		void BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const;
		math::Vector3f Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const;
		math::Vector3f IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const;
		void EndLighting() const;
	private:
		math::Vector3f attenuation_;
		math::Vector3f lightWorldDir_;
		math::Vector3f lightWorldPos_;
		float theta_;	// penumbra angle
		float phi_;		// umbra angle
		float pf_;		// exponent
		mutable float cosTheta_;
		mutable float cosPhi_;
		mutable math::Vector3f eyeWorldPos_;
		mutable const Material* material_;
	};
}

#endif