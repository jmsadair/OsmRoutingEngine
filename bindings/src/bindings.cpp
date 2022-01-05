#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "RoutingEngine.h"

namespace py = pybind11;

PYBIND11_MODULE(OSM, m) {
    py::class_<OSM::RoutingEngine>(m, "RoutingEngine")
            .def(py::init<>())
            .def("loadRoutingData", &OSM::RoutingEngine::loadRoutingData)
            .def("computeRoute", &OSM::RoutingEngine::computeRoute);
}

