//
// Copyright (c) 2022 Slaven Falandys
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef SFL_DETAIL_TO_ADDRESS_HPP_INCLUDED
#define SFL_DETAIL_TO_ADDRESS_HPP_INCLUDED

#include <memory>       // pointer traits
#include <type_traits>  // type traits

namespace sfl
{

namespace dtl
{

//
// Raw pointer overload.
// Obtains a dereferenceable pointer to its argument.
//
template <typename T>
constexpr
T* to_address(T* p) noexcept
{
    static_assert(!std::is_function<T>::value, "not a function pointer");
    return p;
}

//
// Fancy pointer overload.
// Obtains a raw pointer from a fancy pointer.
//
template <typename Pointer>
constexpr
auto to_address(const Pointer& p) noexcept -> typename std::pointer_traits<Pointer>::element_type*
{
    return sfl::dtl::to_address(p.operator->());
}

} // namespace dtl

} // namespace sfl

#endif // SFL_DETAIL_TO_ADDRESS_HPP_INCLUDED
