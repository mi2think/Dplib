/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   20:50
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTexture.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTexture
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Texture
*********************************************************************/
#include "DpTexture.h"

namespace dopixel
{
	Texture::Texture()
		: width_(0)
		, height_(0)
		, format_(PixelFormat::Unknown)
		, mipmapCount_(0)
		, filterMin_(TextureFilter::NearestPoint)
		, filterMag_(TextureFilter::NearestPoint)
		, wrapS_(TextureWrap::Reapeat)
		, wrapT_(TextureWrap::Reapeat)
	{
	}

	Texture::Texture(int width, int height, PixelFormat::Type format)
		: width_(width)
		, height_(height)
		, format_(format)
		, filterMin_(TextureFilter::NearestPoint)
		, filterMag_(TextureFilter::NearestPoint)
		, wrapS_(TextureWrap::Reapeat)
		, wrapT_(TextureWrap::Reapeat)
		, mipmapCount_(1)
	{
		ImageRef image(new Image(width, height, format));
		mipmaps_.push_back(image);
	}

	Texture::Texture(const string& path, bool mipmaps)
		: filterMin_(TextureFilter::NearestPoint)
		, filterMag_(TextureFilter::NearestPoint)
		, wrapS_(TextureWrap::Reapeat)
		, wrapT_(TextureWrap::Reapeat)
	{
		Load(path, mipmaps);
	}

	Texture::Texture(const ImageRef& image)
	{
		SetImage(image);
	}

	Texture::~Texture()
	{
		Clear();
	}

	void Texture::SetImage(const ImageRef& image)
	{
		width_ = image->GetWidth();
		height_ = image->GetHeight();
		format_ = image->GetFormat();
		mipmapCount_ = 1;
		filterMin_ = TextureFilter::NearestPoint;
		filterMag_ = TextureFilter::NearestPoint;
		wrapS_ = TextureWrap::Reapeat;
		wrapT_ = TextureWrap::Reapeat;
		mipmaps_.push_back(image);
	}

	void Texture::Load(const string& path, bool mipmaps)
	{
		Clear();

		// get 4bp RGBA data from file
		ImageRef image = Image::FromFile(path);
		int width = image->GetWidth();
		int height = image->GetHeight();
		ASSERT(image->Valid());

		// texture use float4(RGBA) for convenient
		ImageConverter converter(image.Get());
		ImageRef destImage = converter.Convert(PixelFormat::FLOAT4);

		width_ = width;
		height_ = height;
		format_ = destImage->GetFormat();
		mipmaps_.push_back(destImage);

		if (mipmaps)
			GenMipmaps(destImage);
		else
			mipmapCount_ = 1;
	}

	void Texture::SaveTGA(const string& path, int level)
	{
		ASSERT(level < mipmaps_.size());
		mipmaps_[level]->SaveTGA(path);
	}

	ImageRef Texture::GetMipmap(int level) const
	{
		ASSERT(level >= 0 && level < mipmapCount_);
		return mipmaps_[level];
	}

	void Texture::GenMipmaps(ImageRef image)
	{
		// TODO
	}

	void Texture::Clear()
	{
		mipmaps_.clear();
		mipmapCount_ = 0;
	}

	TextureCache::TextureCache()
	{
	}

	TextureCache::~TextureCache()
	{
		Clear();
	}

	TextureRef TextureCache::Find(const string& name) const
	{
		ASSERT(!name.empty());
		auto it = textures_.find(name);
		if (it != textures_.end())
			return it->second;
		return TextureRef();
	}

	void TextureCache::Add(const TextureRef& texture)
	{
		const auto& name = texture->GetName();
		TextureRef ref = Find(name);
		if (!ref)
		{
			textures_[name] = ref;
		}
	}

	void TextureCache::Clear()
	{
		textures_.clear();
	}
}