#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class TextureWrapApp : public DemoApp
{
public:
	TextureWrapApp() : texture_(new Texture) {}

	virtual void OnCreate() override;

	virtual void OnRender(const Timestep& timestep) override;

	virtual bool OnEvent(const Event& event) override;

	bool OnKeyPressEvent(const KeyPressEvent& keyEvent);

	void CreateCheckerboardImage(int w, int h);
private:
	VertexBufferRef vb_;
	IndexBufferRef	ib_;
	TextureRef texture_;
	Vector3f cameraPos_;
};

void TextureWrapApp::OnCreate()
{
	DemoApp::OnCreate();

	{
		vb_ = new VertexBuffer;
		// position
		Ref<VertexArray3f> positions(new VertexArray3f(4));
		Vector3f* pos = positions->DataAs<Vector3f>();
		pos[0] = Vector3f(-1.0f, 1.0f, 1.0f);
		pos[1] = Vector3f(1.0f, 1.0f, 1.0f);
		pos[2] = Vector3f(1.0f, -1.0f, 1.0f);
		pos[3] = Vector3f(-1.0f, -1.0f, 1.0f);
		vb_->SetPositions(positions);
		// color
		Ref<VertexArray3f> colors(new VertexArray3f(4));
		Vector3f* color = colors->DataAs<Vector3f>();
		color[0] = Vector3f(1, 1, 1);
		color[1] = Vector3f(1, 1, 1);
		color[2] = Vector3f(1, 1, 1);
		color[3] = Vector3f(1, 1, 1);
		vb_->SetColors(colors);
		// uvs
		Ref<VertexArray2f> uvs(new VertexArray2f(4));
		Vector2f* uv = uvs->DataAs<Vector2f>();
		uv[0] = Vector2f(0, 0);
		uv[1] = Vector2f(2, 0);
		uv[2] = Vector2f(2, 2);
		uv[3] = Vector2f(0, 2);
		vb_->SetTexCoords(uvs);
	}

	{
		ib_ = new IndexBuffer(vb_->GetPrimitiveType(), 2);
		unsigned int* indices = ib_->GetData();
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 3;
		indices[3] = 3;
		indices[4] = 1;
		indices[5] = 2;
	}

	// texture
	CreateCheckerboardImage(64, 64);
	texture_->SetWrapS(TextureWrap::ClampToEdge);
	texture_->SetWrapT(TextureWrap::ClampToEdge);

	// camera
	cameraPos_ = Vector3f(0, 0, 0);
	cameraNode_->SetPosition(cameraPos_);

	renderer_->SetShadeMode(ShadeMode::Flat);
	renderer_->SetCameraNode(cameraNode_);
	renderer_->SetTexture(texture_);
}

void TextureWrapApp::CreateCheckerboardImage(int w, int h)
{
	ImageRef image(new Image(w, h, ImageFormat::FLOAT4));
	float* data = (float*)image->GetData();
	float* p = data;
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			bool ih = ((i & 0x8) == 0);
			bool jw = ((j & 0x8) == 0);
			float c = (ih == jw ? 0.0f : 1.0f);
			//ARGB
			*data = 1.0f;
			*(data + 1) = c;
			*(data + 2) = c;
			*(data + 3) = c;
			data += 4;
		}
	}

	texture_->SetImage(image);
}

void TextureWrapApp::OnRender(const Timestep& timestep)
{
	renderer_->DrawPrimitive(vb_, ib_);
}

bool TextureWrapApp::OnEvent(const Event& event)
{
	EventDispatch dispatch(event);
	dispatch.Dispatch(this, &TextureWrapApp::OnKeyPressEvent);
	return dispatch.GetResult();
}

bool TextureWrapApp::OnKeyPressEvent(const KeyPressEvent& keyEvent)
{
	bool update = false;
	int key = keyEvent.GetKey();
	if (key == KEY_KEY_W)
	{
		cameraPos_.z += 0.05f;
		update = true;
	}
	else if (key == KEY_KEY_S)
	{
		cameraPos_.z -= 0.05f;
		update = true;
	}

	if (update)
	{
		cameraNode_->SetPosition(cameraPos_);
		renderer_->SetTransform(Transform::View, cameraNode_->GetViewMatrix());
		renderer_->SetTransform(Transform::Projection, cameraNode_->GetProjectionMatrix());
	}
	return true;
}

int main()
{
	Ref<TextureWrapApp> app(new TextureWrapApp);
	app->Create(400, 400, "Texture Wrap");
	app->Loop();

	return 0;
}