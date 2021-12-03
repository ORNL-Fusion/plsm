#pragma once

#include <plsm/Subpaving.h>

#ifdef PLSM_ENABLE_VTK
#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

namespace plsm
{
namespace test
{
template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
inline void
renderSubpaving(Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>& sp)
{
	auto subpaving = sp.makeMirrorCopy();
	auto tiles = subpaving.getTiles();

	auto numTiles = tiles.extent(0);
	auto points = vtkSmartPointer<vtkPoints>::New();
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType pId = 0;
	for (std::size_t i = 0; i < numTiles; ++i) {
		const auto& region = tiles(i).getRegion();

		if (Dim == 3) {
			points->InsertNextPoint(
				region[0].begin(), region[1].begin(), region[2].begin());
			points->InsertNextPoint(
				region[0].end(), region[1].begin(), region[2].begin());
			points->InsertNextPoint(
				region[0].end(), region[1].end(), region[2].begin());
			points->InsertNextPoint(
				region[0].begin(), region[1].end(), region[2].begin());

			points->InsertNextPoint(
				region[0].begin(), region[1].begin(), region[2].end());
			points->InsertNextPoint(
				region[0].end(), region[1].begin(), region[2].end());
			points->InsertNextPoint(
				region[0].end(), region[1].end(), region[2].end());
			points->InsertNextPoint(
				region[0].begin(), region[1].end(), region[2].end());

			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId);
			cells->InsertCellPoint(pId + 1);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 1);
			cells->InsertCellPoint(pId + 2);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 2);
			cells->InsertCellPoint(pId + 3);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 3);
			cells->InsertCellPoint(pId);

			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 4);
			cells->InsertCellPoint(pId + 5);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 5);
			cells->InsertCellPoint(pId + 6);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 6);
			cells->InsertCellPoint(pId + 7);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 7);
			cells->InsertCellPoint(pId + 4);

			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 0);
			cells->InsertCellPoint(pId + 4);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 1);
			cells->InsertCellPoint(pId + 5);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 2);
			cells->InsertCellPoint(pId + 6);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 3);
			cells->InsertCellPoint(pId + 7);

			pId += 8;
		}
		else if (Dim == 2) {
			points->InsertNextPoint(region[0].begin(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].end(), 0.0);
			points->InsertNextPoint(region[0].begin(), region[1].end(), 0.0);

			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId);
			cells->InsertCellPoint(pId + 1);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 1);
			cells->InsertCellPoint(pId + 2);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 2);
			cells->InsertCellPoint(pId + 3);
			cells->InsertNextCell(2);
			cells->InsertCellPoint(pId + 3);
			cells->InsertCellPoint(pId);

			pId += 4;
		}
		else {
			throw std::runtime_error("Unsupported dimensionality");
		}
	}

	auto pd = vtkSmartPointer<vtkPolyData>::New();
	pd->SetPoints(points);
	pd->SetLines(cells);

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pd);
	auto actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(0, 0, 0);
	static auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->RemoveAllViewProps();
	renderer->AddActor(actor);
	renderer->SetBackground(1, 1, 1);
	static auto interactor = [=]() {
		auto window = vtkSmartPointer<vtkRenderWindow>::New();
		window->AddRenderer(renderer);
		auto ia = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		ia->SetRenderWindow(window);
		return ia;
	}();
	interactor->GetRenderWindow()->Render();
	interactor->Start();
}
} // namespace test
} // namespace plsm

#else

namespace plsm
{
namespace test
{
template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
inline void
renderSubpaving(Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>& subpaving)
{
	std::cout << "\nNumber of Tiles: " << subpaving.getNumberOfTiles()
			  << std::endl;
}
} // namespace test
} // namespace plsm

#endif

namespace plsm
{
namespace test
{
template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
inline void
plot(Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>& subpaving)
{
	auto tiles = subpaving.getTiles();
	std::ofstream ofs("gp.txt");
	for (auto i : makeIntervalRange(tiles.extent(0))) {
		const auto& region = tiles(i).getRegion();
		ofs << "\n";
		ofs << region[0].begin() << " " << region[1].begin() << "\n";
		ofs << region[0].end() << " " << region[1].begin() << "\n";
		ofs << region[0].end() << " " << region[1].end() << "\n";
		ofs << region[0].begin() << " " << region[1].end() << "\n";
		ofs << region[0].begin() << " " << region[1].begin() << "\n";
		ofs << "\n";
		double q01 = 0.25 * region[0].begin() + 0.75 * region[0].end();
		double q03 = 0.75 * region[0].begin() + 0.25 * region[0].end();
		double q11 = 0.25 * region[1].begin() + 0.75 * region[1].end();
		double q13 = 0.75 * region[1].begin() + 0.25 * region[1].end();
		ofs << q01 << " " << q11 << "\n";
		ofs << q03 << " " << q13 << "\n";
		ofs << "\n";
		ofs << q01 << " " << q13 << "\n";
		ofs << q03 << " " << q11 << "\n";
	}
}
} // namespace test
} // namespace plsm
