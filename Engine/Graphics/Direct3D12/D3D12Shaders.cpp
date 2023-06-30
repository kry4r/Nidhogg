#include "D3D12Shaders.h"
#include "Content\ContentLoader.h"

namespace nidhog::graphics::d3d12::shaders
{
	namespace
	{
        typedef struct compiled_shader
        {
            u64         size;
            const u8*   byte_code;
        } const* compiled_shader_ptr;

        // 该数组中的每个元素都指向着色器 blob 内的偏移量
        compiled_shader_ptr engine_shaders[engine_shader::count]{};

        // 这是包含所有已编译引擎着色器的内存块.
        // blob 是一个着色器字节代码数组，由 u64 大小和字节数组组成
        std::unique_ptr<u8[]> shaders_blob{};

        bool load_engine_shaders()
        {
            assert(!shaders_blob);
            u64 size{ 0 };
            bool result{ content::load_engine_shaders(shaders_blob, size) };
            assert(shaders_blob && size);

            u64 offset{ 0 };
            u32 index{ 0 };
            while (offset < size && result)
            {
                assert(index < engine_shader::count);
                compiled_shader_ptr& shader{ engine_shaders[index] };
                assert(!shader);
                result &= index < engine_shader::count && !shader;
                if (!result) break;
                shader = reinterpret_cast<const compiled_shader_ptr>(&shaders_blob[offset]);
                offset += sizeof(u64) + shader->size;
                ++index;
            }
            assert(offset == size && index == engine_shader::count);

            return result;
        }
	}//匿名namespace


	bool initialize()
	{
		return load_engine_shaders();
	}

	void shutdown()
	{
        for (u32 i{ 0 }; i < engine_shader::count; ++i)
        {
            engine_shaders[i] = {};
        }
        shaders_blob.reset();
	}

	D3D12_SHADER_BYTECODE get_engine_shader(engine_shader::id id)
	{
        assert(id < engine_shader::count);
        const compiled_shader_ptr shader{ engine_shaders[id] };
        assert(shader && shader->size);
        return { &shader->byte_code, shader->size };
	}
}
