#pragma once

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
template <typename TScalar, std::size_t Dim, typename TItemData>
void
renderSubpaving(Subpaving<TScalar, Dim, TItemData>& subpaving)
{
	subpaving.syncTiles(onHost);
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
		}
		else if (Dim == 2) {
			points->InsertNextPoint(region[0].begin(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].end(), 0.0);
			points->InsertNextPoint(region[0].begin(), region[1].end(), 0.0);

			points->InsertNextPoint(region[0].begin(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].begin(), 0.0);
			points->InsertNextPoint(region[0].end(), region[1].end(), 0.0);
			points->InsertNextPoint(region[0].begin(), region[1].end(), 0.0);
		}
		else {
			throw std::runtime_error("Unsupported dimensionality");
		}

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

	auto pd = vtkSmartPointer<vtkPolyData>::New();
	pd->SetPoints(points);
	pd->SetLines(cells);

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pd);
	auto actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(0, 0, 0);
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(1, 1, 1);
	auto window = vtkSmartPointer<vtkRenderWindow>::New();
	window->AddRenderer(renderer);
	auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(window);
	window->Render();
	interactor->Start();
}
} // namespace test
} // namespace plsm
#endif
