#pragma once

#include "./math.hpp"

namespace maya
{

// Abstract class for rendering resources.
class RenderResource
{
public:

	// Uninitialized.
	RenderResource(void);

	// Destructor.
	virtual ~RenderResource() = default;

	// Initialize resources.
	virtual void Init(class RenderContext& rc);

	// Destroy resources.
	virtual void Free();

	// No copy constructs.
	RenderResource(RenderResource const&) = delete;
	RenderResource& operator=(RenderResource const&) = delete;

	// Returns the native id.
	inline MAYA_STL uint32_t GetNativeId() const { return nativeid; };

protected:

	RenderContext* rc;
	MAYA_STL uint32_t nativeid;
};

// Graphics context for rendering.
class RenderContext
{
public:

	// Set this rendering context to the current.
	void BeginContext();

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
	inline VertexArray* GetInput() { return input; }

	// Get the current shader prgoram.
	inline ShaderProgram* GetProgram() { return program; }

	// Get the current texture to a slot.
	inline Texture* GetTexture(int slot) { return textures[slot]; }

	// Settings for rendering.
	enum Options
	{
		BLENDING,
		SCISSOR_TEST,
	};

	// Enable a rendering setting.
	void Enable(Options set);

	// Disable a rendering setting.
	void Disable(Options set);

	// Return true if a setting is enabled.
	inline bool IsEnabled(Options set) const { return settings & (1 << set); }

	// Usable blending modes
	enum BlendMode
	{
		NO_BLEND,
		ALPHA_BLEND,
		ADDITIVE_BLEND,
		MULTIPLICATIVE_BLEND
	};

	// Set a blend mode.
	void SetBlendMode(BlendMode bm);

	// Get the current blend mode.
	inline BlendMode GetBlendMode() const { return blendmode; }

	// Draw the current bounded setup.
	void DrawSetup();

	// Get the maximum number of texture slots available.
	inline MAYA_STL size_t GetMaxTextureSlots() const { return textures.size(); }

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

	RenderContext() = default;
	void Init(class Window* window);
	void Free();
};

}