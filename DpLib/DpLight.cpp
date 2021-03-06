/********************************************************************
	created:	2015/11/20
	created:	20:11:2015   20:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLight.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLight
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Light
*********************************************************************/
#include "DpLight.h"
#include "DpMaterial.h"

namespace dopixel
{
	Light::Light(
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		float range)
		: ambient_(ambient)
		, diffuse_(diffuse)
		, specular_(specular)
		, range_(range)
	{
	}

	Light::~Light()
	{
	}

	DirectionalLight::DirectionalLight(
		const math::Vector3f& lightWorldDir,
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		float range)
		: Light(ambient, diffuse, specular, range)
		, lightWorldDir_(lightWorldDir)
	{
		lightWorldDir_.Normalize();
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	void DirectionalLight::BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const
	{
		invLightWorldDir_ = -lightWorldDir_;
		eyeWorldPos_ = eyeWorldPos;
		material_ = material.Get();
	}

	math::Vector3f DirectionalLight::Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(invLightWorldDir_, normal);
		if (dp > EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;
		}
		return color;
	}

	math::Vector3f DirectionalLight::IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(invLightWorldDir_, normal);
		if (dp > EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;

			// specular color
			math::Vector3f posToEye = (eyeWorldPos_ - pos);
			posToEye.Normalize();
			math::Vector3f lightReflect = (lightWorldDir_ - 2 * normal * DotProduct(lightWorldDir_, normal));
			lightReflect.Normalize();
			float k = DotProduct(posToEye, lightReflect);
			if (k > EPSILON_E5)
			{
				k = pow(k, material_->GetShininess());
				math::Vector3f specularColor = material_->GetColor(ColorUsage::Specular);
				specularColor *= specular_;

				color += specularColor * k;
			}
		}
		return color;
	}

	void DirectionalLight::EndLighting() const
	{
		material_ = nullptr;
		invLightWorldDir_.Zero();
	}

	PointLight::PointLight(
		const math::Vector3f& lightWorldPos,
		const math::Vector3f& attenuation,
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		float range)
		: Light(ambient, diffuse, specular, range)
		, lightWorldPos_(lightWorldPos)
		, attenuation_(attenuation)
	{
	}

	PointLight::~PointLight()
	{
	}

	void PointLight::BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const
	{
		eyeWorldPos_ = eyeWorldPos;
		material_ = material.Get();
	}

	math::Vector3f PointLight::Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// attenuation
		math::Vector3f lightWorldDir = pos - lightWorldPos_;
		float distance = lightWorldDir.Length();
		float atten = attenuation_.x + attenuation_.y * distance + attenuation_.z * distance * distance;
		lightWorldDir.Normalize();

		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(-lightWorldDir, normal);
		if (dp > EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;
		}

		return color / atten;
	}

	math::Vector3f PointLight::IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// attenuation
		math::Vector3f lightToPos = pos - lightWorldPos_;
		float distance = lightToPos.Length();
		float atten = attenuation_.x + attenuation_.y * distance + attenuation_.z * distance * distance;
		lightToPos.Normalize();

		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(-lightToPos, normal);
		if (dp > EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;

			// specular color
			math::Vector3f posToEye = (eyeWorldPos_ - pos);
			posToEye.Normalize();
			math::Vector3f lightReflect = (lightToPos - 2 * normal * DotProduct(lightToPos, normal));
			lightReflect.Normalize();
			float k = DotProduct(posToEye, lightReflect);
			if (k > EPSILON_E5)
			{
				k = pow(k, material_->GetShininess());
				math::Vector3f specularColor = material_->GetColor(ColorUsage::Specular);
				specularColor *= specular_;

				color += specularColor * k;
			}
		}

		return color / atten;
	}

	void PointLight::EndLighting() const
	{
		material_ = nullptr;
	}

	SpotLight::SpotLight(
		const math::Vector3f& lightWorldDir,
		const math::Vector3f& lightWorldPos,
		const math::Vector3f& attenuation,
		float theta,
		float phi,
		float pf,
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		float range)
		: Light(ambient, diffuse, specular, range)
		, lightWorldPos_(lightWorldPos)
		, lightWorldDir_(lightWorldDir)
		, attenuation_(attenuation)
		, theta_(theta)
		, phi_(phi)
		, pf_(pf)
	{
		cosTheta_ = math::Cos(angle2radian(theta));
		cosPhi_ = math::Cos(angle2radian(phi));
		lightWorldDir_.Normalize();
	}

	SpotLight::~SpotLight()
	{
	}

	void SpotLight::BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const
	{
		eyeWorldPos_ = eyeWorldPos;
		material_ = material.Get();
	}

	math::Vector3f SpotLight::Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		math::Vector3f lightToPos = pos - lightWorldPos_;
		float distance = lightToPos.Length();
		lightToPos.Normalize();

		// attenuation
		float atten = attenuation_.x + attenuation_.y * distance + attenuation_.z * distance * distance;

		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(lightToPos, lightWorldDir_);
		if (dp <= cosPhi_)
		{
			return color / atten;
		}
		else
		{
			float k = DotProduct(-lightToPos, normal);
			if (k > EPSILON_E5)
			{
				// diffuse color
				math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
				diffuseColor *= diffuse_;
				diffuseColor *= k;

				if (dp < cosTheta_)
				{
					float a = cosTheta_ - dp;
					float b = cosPhi_ - dp;
					diffuseColor *= pow(a / b, pf_);
				}

				color += diffuseColor;
			}
		}

		return color / atten;
	}

	math::Vector3f SpotLight::IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		math::Vector3f lightToPos = pos - lightWorldPos_;
		float distance = lightToPos.Length();
		lightToPos.Normalize();

		// attenuation
		float atten = attenuation_.x + attenuation_.y * distance + attenuation_.z * distance * distance;

		// ambient color
		math::Vector3f color = material_->GetColor(ColorUsage::Ambient);
		color *= ambient_;

		float dp = DotProduct(lightToPos, lightWorldDir_);
		if (dp <= cosPhi_)
		{
			return color / atten;
		}
		else
		{
			float k = DotProduct(-lightToPos, normal);
			if (k > EPSILON_E5)
			{
				// diffuse color
				math::Vector3f diffuseColor = material_->GetColor(ColorUsage::Diffuse);
				diffuseColor *= diffuse_;
				diffuseColor *= k;

				if (dp < cosTheta_)
				{
					float a = cosTheta_ - dp;
					float b = cosPhi_ - dp;
					diffuseColor *= pow(a / b, pf_);
				}

				color += diffuseColor;

				// specular color
				math::Vector3f posToEye = (eyeWorldPos_ - pos);
				posToEye.Normalize();
				math::Vector3f lightReflect = (lightToPos - 2 * normal * DotProduct(lightToPos, normal));
				lightReflect.Normalize();
				float w = DotProduct(posToEye, lightReflect);
				if (w > EPSILON_E5)
				{
					w = pow(w, material_->GetShininess());
					math::Vector3f specularColor = material_->GetColor(ColorUsage::Specular);
					specularColor *= specular_;

					color += specularColor * w;
				}
			}
		}

		return color / atten;
	}

	void SpotLight::EndLighting() const
	{
		material_ = nullptr;
	}
}