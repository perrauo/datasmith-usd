// DatasmithUsdPlugin.cpp
#include "DatasmithUsd.h"
#include "pybind11/pybind11.h"

#include <iostream>

void DatasmithUsd::Export() {
    // Your export functionality goes here
    //std::cout << "Datasmith Export!" << std::endl;
}

namespace py = pybind11;

PYBIND11_MODULE(DatasmithUsd, m) {
    py::class_<DatasmithUsd>(m, "DatasmithUsd")
        .def(py::init<>())
        .def("export", &DatasmithUsd::Export);
}