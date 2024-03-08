#pragma once

#include "./math.hpp"

namespace maya
{

class RenderResource
{
public:

	RenderResource(class RenderContext& rc);
	virtual ~RenderResource() = default;

	RenderResource(RenderResource const&) = delete;
	RenderResource& operator=(RenderResource const&) = delete;

	virtual void CleanUp();

protected:
	RenderContext& rc;
};

class RenderContext
{
public:

	void Begin();
	void ClearBuffer();

	void SetInput(class VertexArray* vao);
	void SetProgram(class ShaderProgram* program);
	void SetTexture(class Texture* tex, int slot);

	void DrawSetup();

private:

	class Window* window;
	stl::uset<RenderResource*> resources;

	class VertexArray* crntvao;
	class ShaderProgram* crntprogram;

	friend class Window;
	friend class RenderResource;

	RenderContext();
	void CleanAll();
};

}