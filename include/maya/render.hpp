#pragma once

#include "./math.hpp"

namespace maya
{

// Abstract class for rendering resources.
class RenderResource
{
public:

	// Add itself to the collection.
	RenderResource(class RenderContext& rc);

	// Is expected to call Destroy() in the child implementation.
	virtual ~RenderResource() = default;

	// No copy constructs.
	RenderResource(RenderResource const&) = delete;
	RenderResource& operator=(RenderResource const&) = delete;

	// Returns the native id.
	virtual unsigned GetNativeId() const = 0;

protected:

	RenderContext& rc;
	friend class RenderContext;

	// Called by the destructor or RenderContext clean up.
	// Overriders are expected to include this destroy function.
	virtual void Destroy();
};

// Graphics context for rendering.
class RenderContext
{
public:

	// Set this rendering context to the current.
	void Begin();

	// Get the current context.
	static RenderContext* CurrentContext();

	// Clear color buffers.
	void ClearBuffer();

	// Set an input for rendering, require non null for DrawSetup().
	void SetInput(class VertexArray* vao);

	// Set a shader program for the pipelines, require non null for DrawSetup().
	void SetProgram(class ShaderProgram* program);

	// Bind a texture to a specific slot, optional.
	void SetTexture(class Texture* tex, int slot);

	// Get the current vertex array.
	VertexArray* GetInput();

	// Get the current shader prgoram.
	ShaderProgram* GetProgram();

	// Get the current texture to a slot.
	Texture* GetTexture(int slot);

	// Settings for rendering.
	enum Setting
	{
		Blending,
		ScissorTest,
	};

	// Enable a rendering setting.
	void Enable(Setting set);

	// Disable a rendering setting.
	void Disable(Setting set);

	// Return true if a setting is enabled.
	bool IsEnabled(Setting set) const;

	// Usable blending modes
	enum BlendMode
	{
		NoBlend,
		AlphaBlend,
		AdditiveBlend,
		MulitplicativeBlend
	};

	// Set a blend mode.
	void SetBlendMode(BlendMode bm);

	// Get the current blend mode.
	BlendMode GetBlendMode();

	// Draw the current bounded setup.
	void DrawSetup();

	// Get the maximum number of texture slots available.
	int GetMaxTextureSlots() const;

	// Tell SyncWithThreads to wait even no threads are waiting.
	struct QuietWait {
		RenderContext& rc;
		QuietWait(RenderContext& rc);
		~QuietWait();
	};

	// Synchronous render related execution.
	void WaitForSyncExec(stl::fnptr<void()> const& exec);

	// Wait in the main thread for WaitForSyncExec.
	void SyncWithThreads(float maxwait);

private:

	class Window* window;
	stl::hashset<RenderResource*> resources;

	VertexArray* input;
	ShaderProgram* program;
	stl::list<Texture*> textures;
	unsigned settings;
	BlendMode blendmode;

	friend class Window;
	friend class RenderResource;

	// multithread support for synchronization.
	MAYA_STL mutex mut;
	std::condition_variable cv, cv0, cv1;
	MAYA_STL thread::id threadid;
	stl::fnptr<void()> execsync;
	std::atomic<unsigned> num_wait, num_quiet_wait;

	RenderContext();
	void Init();
	void DestroyAll();
};

}