#pragma once

//Kokkos
#include <Kokkos_Core.hpp>

namespace plsm
{
struct OnHost
{
};


struct OnDevice
{
};


constexpr OnHost onHost{};
constexpr OnDevice onDevice{};


namespace detail
{
template <typename TDualView, typename TContext>
struct ContextualView
{
};


template <typename TDualView>
struct ContextualView<TDualView, ::plsm::OnHost>
{
    using Type = typename TDualView::t_host;
};


template <typename TDualView>
struct ContextualView<TDualView, ::plsm::OnDevice>
{
    using Type = typename TDualView::t_dev;
};


template <typename TDualView, typename TContext>
using ContextualViewType = typename ContextualView<TDualView, TContext>::Type;


template <typename TDualView>
ContextualViewType<TDualView, ::plsm::OnHost>&
getContextualView(TDualView& dualView, ::plsm::OnHost)
{
    return dualView.h_view;
}


template <typename TDualView>
ContextualViewType<TDualView, ::plsm::OnDevice>&
getContextualView(TDualView& dualView, ::plsm::OnDevice)
{
    return dualView.d_view;
}


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
}
}
