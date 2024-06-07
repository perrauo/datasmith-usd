// DatasmithUsdPlugin.cpp
#include "DatasmithUsd.h"

#include "USDIncludesStart.h"
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <boost/python.hpp>
#include "USDIncludesEnd.h"

#include <iostream>
#include <string>

PXR_NAMESPACE_OPEN_SCOPE

void FDatasmithUsd::Export(const UsdStageRefPtr& InStage) {
	auto Prim = InStage->GetDefaultPrim();	
    std::cout << Prim.GetName().data() << std::endl;
}

PXR_NAMESPACE_CLOSE_SCOPE

BOOST_PYTHON_MODULE(DatasmithUsd)
{
	using namespace boost::python;
	class_<pxr::FDatasmithUsd>("DatasmithUsd")
		.def(init<>()) // Default constructor
		.def("export", &pxr::FDatasmithUsd::Export);
}