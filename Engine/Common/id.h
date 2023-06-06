#pragma once
#pragma once
#include "CommonHeaders.h"

namespace nidhog::id {

    using id_type = u32; //32λid
    namespace internal{
        constexpr u32 generation_bits{ 8 }; //������λ��generationλ
        constexpr u32 index_bits{ sizeof(id_type) * 8 - generation_bits };//id����λ
        constexpr id_type index_mask{ (id_type{1} << index_bits) - 1 };//����λ����
        constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 };//����λ����
    }//internal namespace
    
    constexpr id_type invalid_id{ id_type(-1) };//id����
    constexpr u32 min_deleted_elements{ 1024 };//�ڿ�ʼд��ɾ��Ԫ��λ��֮ǰɾ����Ԫ������

    using generation_type = std::conditional_t<internal::generation_bits <= 16, std::conditional_t<internal::generation_bits <= 8, u8, u16>, u32>;//�����generation�����͡�
    //�������������жϣ���ֹ�������
    static_assert(sizeof(generation_type) * 8 >= internal::generation_bits);
    static_assert((sizeof(id_type) - sizeof(generation_type)) > 0);

    constexpr bool
        is_valid(id_type id)
    {
        return id != invalid_id;
    }

    constexpr id_type
        index(id_type id)
    {
        id_type index{ id & internal::index_mask };
        assert(index != internal::index_mask);
        return index;
    }

    constexpr id_type
        generation(id_type id)
    {
        return (id >> internal::index_bits) & internal::generation_mask;
    }

    constexpr id_type
        new_generation(id_type id)//�����Եõ�id
    {
        const id_type generation{ id::generation(id) + 1 };
        assert(generation < (((u64)1 << internal::generation_bits) - 1));//�������ֵ�������ж�
        return index(id) | (generation << internal::index_bits);
    }

#if _DEBUG
    namespace internal {
        struct id_base
        {
            constexpr explicit id_base(id_type id) : _id{ id } {}
            constexpr operator id_type() const { return _id; }
        private:
            id_type _id;
        };
    }//internal namespace
#define DEFINE_TYPED_ID(name)                                   \
        struct name final : id::internal::id_base               \
        {                                                       \
            constexpr explicit name(id::id_type id)             \
                : id_base{ id } {}                              \
            constexpr name() : id_base{ 0 } {}                  \
        };
#else
    //һ�������̳�id type�ĺ�
#define DEFINE_TYPED_ID(name) using name = id::id_type;
#endif

}