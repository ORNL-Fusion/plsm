#pragma once

#include <Kokkos_Core.hpp>

namespace plsm
{
/*!
 * @brief Tag type for host memory space
 */
struct OnHost
{
};


/*!
 * @brief Tag type for device memory space
 */
struct OnDevice
{
};


//! Tag instance for host memory space
inline constexpr OnHost onHost{};
//! Tag instance for device memory space
inline constexpr OnDevice onDevice{};


namespace detail
{
/*!
 * @brief Determine the view type appropriate for the given memory space
 */
template <typename TDualView, typename TContext>
struct ContextualViewTypeHelper
{
};


/*! @cond */
template <typename TDualView>
struct ContextualViewTypeHelper<TDualView, ::plsm::OnHost>
{
    using Type = typename TDualView::t_host;
};


template <typename TDualView>
struct ContextualViewTypeHelper<TDualView, ::plsm::OnDevice>
{
    using Type = typename TDualView::t_dev;
};
/*! @endcond */


/*!
 * @brief Determine the part of a Kokkos::DualView appropriate for the given
 * memory space
 */
template <typename TDualView, typename TContext>
using ContextualViewType =
    typename ContextualViewTypeHelper<TDualView, TContext>::Type;


//@{
/*!
 * @brief Get the part of a Kokkos::DualView appropriate for the given memory
 * space
 */
template <typename TDualView>
KOKKOS_INLINE_FUNCTION
const ContextualViewType<TDualView, ::plsm::OnHost>&
getContextualView(const TDualView& dualView, ::plsm::OnHost)
{
    return dualView.h_view;
}


template <typename TDualView>
KOKKOS_INLINE_FUNCTION
const ContextualViewType<TDualView, ::plsm::OnDevice>&
getContextualView(const TDualView& dualView, ::plsm::OnDevice)
{
    return dualView.d_view;
}
//@}


//@{
/*!
 * @brief Syncronize a dual view in the specified direction
 */
template <typename TDualView>
void
syncUpdate(TDualView& dualView, ::plsm::OnHost)
{
    dualView.modify_device();
    Kokkos::resize(dualView.h_view, dualView.d_view.extent(0));
    dualView.sync_host();
}


template <typename TDualView>
void
syncUpdate(TDualView& dualView, ::plsm::OnDevice)
{
    dualView.modify_host();
    Kokkos::resize(dualView.d_view, dualView.h_view.extent(0));
    dualView.sync_device();
}
//@}
}
}
