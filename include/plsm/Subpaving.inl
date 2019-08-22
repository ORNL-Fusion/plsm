#pragma once

//std
#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>
#include <utility>
//plsm
#include <plsm/IntervalRange.h>
#include <plsm/detail/Refiner.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim, typename TItemData>
Subpaving<TScalar, Dim, TItemData>::Subpaving(const RegionType& region,
        const std::vector<SubdivisionRatioType>& subdivisionRatios)
    :
    _zones("zones", 1),
    _tiles("tiles", 1),
    _rootRegion(region),
    _subdivisionInfos("Subdivision Infos")
{
    processSubdivisionRatios(subdivisionRatios);

    _zones.h_view(0) = ZoneType{_rootRegion, 0};
    _zones.modify_host();

    _tiles.h_view(0) = TileType{_rootRegion, 0};
    _tiles.modify_host();

    _refinementDepth = 1;

    _zones.sync_device();
    _tiles.sync_device();
}


template <typename TScalar, std::size_t Dim, typename TItemData>
void
Subpaving<TScalar, Dim, TItemData>::processSubdivisionRatios(
    const std::vector<SubdivisionRatioType>& subdivRatios)
{
    auto subdivisionRatios = subdivRatios;

    auto elementWiseProduct =
        [](const SubdivisionRatioType& a, const SubdivisionRatioType& b)
        {
            SubdivisionRatioType ret;
            for (auto i : makeIntervalRange(Dim)) {
                ret[i] = a[i] * b[i];
            }
            return ret;
        };
    auto ratioProduct =
        std::accumulate(next(begin(subdivisionRatios)), end(subdivisionRatios),
            subdivisionRatios.front(), elementWiseProduct);

    auto getIntervalLength =
        [](const IntervalType& ival) { return ival.length(); };
    std::array<typename IntervalType::SizeType, Dim> extents;
    std::transform(begin(_rootRegion), end(_rootRegion), begin(extents),
        getIntervalLength);

    //FIXME: infinite loop when ratios do not evenly divide extents
    for (;;) {
        bool needAnotherLevel = false;
        auto newRatio = SubdivisionRatioType::filled(1);
        for (auto i : makeIntervalRange(Dim)) {
            if (ratioProduct[i] < extents[i]) {
                newRatio[i] = subdivisionRatios.back()[i];
                needAnotherLevel = true;
            }
        }

        if (!needAnotherLevel) {
            break;
        }

        subdivisionRatios.push_back(newRatio);
        ratioProduct = elementWiseProduct(ratioProduct, newRatio);
    }

    for (auto i : makeIntervalRange(Dim)) {
        if (ratioProduct[i] != extents[i]) {
            throw std::invalid_argument(
                "Subpaving: subdivision ratios given for dimension " +
                std::to_string(i) + " (" + std::to_string(ratioProduct[i]) +
                ") do not match extent for that dimension (" +
                std::to_string(extents[i]) + ")");
        }
    }

    Kokkos::resize(_subdivisionInfos.h_view, subdivisionRatios.size());
    std::copy(begin(subdivisionRatios), end(subdivisionRatios),
        _subdivisionInfos.h_view.data());
    _subdivisionInfos.modify_host();
    Kokkos::resize(_subdivisionInfos.d_view, subdivisionRatios.size());
    _subdivisionInfos.sync_device();
}


template <typename TScalar, std::size_t Dim, typename TItemData>
template <typename TRefinementDetector>
void
Subpaving<TScalar, Dim, TItemData>::refine(TRefinementDetector&& detector)
{
    detail::makeRefiner(*this, std::forward<TRefinementDetector>(detector))();
}


// template <typename TScalar, std::size_t Dim, typename TItemData>
// void
// Subpaving<TScalar, Dim, TItemData>::refine(const RefinementSpec& refineSpec)
// {
//     detail::makeRefiner(*this, refineSpec)();
// }


// template <typename TScalar, std::size_t Dim, typename TItemData>
// template <typename TSelector>
// void
// Subpaving<TScalar, Dim, TItemData>::refine(const RefinementSpec& refineSpec,
//     TSelector selector)
// {
//     detail::makeRefiner(*this, refineSpec, selector)();
// }


template <typename TScalar, std::size_t Dim, typename TItemData>
void
Subpaving<TScalar, Dim, TItemData>::plot()
{
    _tiles.modify_device();
    Kokkos::resize(_tiles.h_view, _tiles.d_view.extent(0));
    _tiles.sync_host();

    std::ofstream ofs("gp.txt");
    for (auto i : makeIntervalRange(_tiles.h_view.extent(0))) {
        const auto& region = _tiles.h_view(i).getRegion();
        ofs << "\n";
        ofs << region[0].begin() << " " << region[1].begin() << "\n";
        ofs << region[0].end() << " " << region[1].begin() << "\n";
        ofs << region[0].end() << " " << region[1].end() << "\n";
        ofs << region[0].begin() << " " << region[1].end() << "\n";
        ofs << region[0].begin() << " " << region[1].begin() << "\n";
        ofs << "\n";
        double q01 = 0.25*region[0].begin() + 0.75*region[0].end();
        double q03 = 0.75*region[0].begin() + 0.25*region[0].end();
        double q11 = 0.25*region[1].begin() + 0.75*region[1].end();
        double q13 = 0.75*region[1].begin() + 0.25*region[1].end();
        ofs << q01 << " " << q11 << "\n";
        ofs << q03 << " " << q13 << "\n";
        ofs << "\n";
        ofs << q01 << " " << q13 << "\n";
        ofs << q03 << " " << q11 << "\n";
    }
}
}


#ifdef PLSM_ENABLE_VTK
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
namespace plsm
{
template <typename TScalar, std::size_t Dim, typename TItemData>
void
Subpaving<TScalar, Dim, TItemData>::render()
{
    _tiles.modify_device();
    Kokkos::resize(_tiles.h_view, _tiles.d_view.extent(0));
    _tiles.sync_host();

    auto numTiles = _tiles.h_view.extent(0);
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType pId = 0;
    for (std::size_t i = 0; i < numTiles; ++i) {
        const auto& region = _tiles.h_view(i).getRegion();

        points->InsertNextPoint(region[0].begin(), region[1].begin(), region[2].begin());
        points->InsertNextPoint(region[0].end(), region[1].begin(), region[2].begin());
        points->InsertNextPoint(region[0].end(), region[1].end(), region[2].begin());
        points->InsertNextPoint(region[0].begin(), region[1].end(), region[2].begin());

        points->InsertNextPoint(region[0].begin(), region[1].begin(), region[2].end());
        points->InsertNextPoint(region[0].end(), region[1].begin(), region[2].end());
        points->InsertNextPoint(region[0].end(), region[1].end(), region[2].end());
        points->InsertNextPoint(region[0].begin(), region[1].end(), region[2].end());

        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId);
        cells->InsertCellPoint(pId+1);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+1);
        cells->InsertCellPoint(pId+2);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+2);
        cells->InsertCellPoint(pId+3);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+3);
        cells->InsertCellPoint(pId);

        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+4);
        cells->InsertCellPoint(pId+5);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+5);
        cells->InsertCellPoint(pId+6);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+6);
        cells->InsertCellPoint(pId+7);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+7);
        cells->InsertCellPoint(pId+4);

        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+0);
        cells->InsertCellPoint(pId+4);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+1);
        cells->InsertCellPoint(pId+5);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+2);
        cells->InsertCellPoint(pId+6);
        cells->InsertNextCell(2);
        cells->InsertCellPoint(pId+3);
        cells->InsertCellPoint(pId+7);

        pId += 8;
    }

    auto pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetPoints(points);
    pd->SetLines(cells);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(pd);
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    auto window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);
    window->Render();
    interactor->Start();
}
}
#endif
