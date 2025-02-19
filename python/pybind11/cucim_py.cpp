/*
 * Copyright (c) 2020, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cucim_py.h"
#include "cucim_pydoc.h"
#include "io/init.h"
#include "filesystem/init.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <fmt/format.h>
#include <fmt/ranges.h>

using namespace pybind11::literals;
namespace py = pybind11;

namespace cucim
{

static const std::unordered_map<uint8_t, const char*> g_dldata_typecode{
    { kDLInt, "DLInt" },
    { kDLUInt, "DLUInt" },
    { kDLFloat, "DLFloat" },
    { kDLBfloat, "DLBfloat" },
};

PYBIND11_MODULE(_cucim, m)
{

    // clang-format off
#ifdef CUCIM_VERSION
#  define XSTR(x) STR(x)
#  define STR(x) #x
    // Set version
    m.attr("__version__") = XSTR(CUCIM_VERSION);
#endif
    // clang-format on
    // Get/set plugin root path
    m.def("_get_plugin_root", &get_plugin_root);
    m.def("_set_plugin_root", &set_plugin_root);

    // Submodule: io
    auto m_io = m.def_submodule("io");
    io::init_io(m_io);

    // Submodule: filesystem
    auto m_fs = m.def_submodule("filesystem");
    filesystem::init_filesystem(m_fs);

    // Data structures
    py::enum_<DLDataTypeCode>(m, "DLDataTypeCode") //
        .value("DLInt", kDLInt) //
        .value("DLUInt", kDLUInt) //
        .value("DLFloat", kDLFloat) //
        .value("DLBfloat", kDLBfloat);

    py::class_<DLDataType>(m, "DLDataType") //
        .def(py::init([](DLDataTypeCode code, uint8_t bits, uint16_t lanes) {
                 auto ctr = std::make_unique<DLDataType>();
                 ctr->code = static_cast<uint8_t>(code);
                 ctr->bits = bits;
                 ctr->lanes = lanes;
                 return ctr;
             }),
             doc::DLDataType::doc_DLDataType, py::call_guard<py::gil_scoped_release>())
        .def_readonly("code", &DLDataType::code, doc::DLDataType::doc_code, py::call_guard<py::gil_scoped_release>()) //
        .def_readonly("bits", &DLDataType::bits, doc::DLDataType::doc_bits, py::call_guard<py::gil_scoped_release>()) //
        .def_readonly("lanes", &DLDataType::lanes, doc::DLDataType::doc_lanes, py::call_guard<py::gil_scoped_release>()) //
        .def(
            "__repr__",
            [](const DLDataType& dtype) {
                return fmt::format("<cucim.DLDataType code:{}({}) bits:{} lanes:{}>", g_dldata_typecode.at(dtype.code),
                                   dtype.code, dtype.bits, dtype.lanes);
            },
            py::call_guard<py::gil_scoped_release>());

    py::class_<CuImage, std::shared_ptr<CuImage>>(m, "CuImage") //
        .def(py::init<const std::string&>(), doc::CuImage::doc_CuImage, py::call_guard<py::gil_scoped_release>(), //
             py::arg("path")) //
        .def_property("path", &CuImage::path, nullptr, doc::CuImage::doc_path, py::call_guard<py::gil_scoped_release>()) //
        .def_property("is_loaded", &CuImage::is_loaded, nullptr, doc::CuImage::doc_is_loaded,
                      py::call_guard<py::gil_scoped_release>()) //
        .def_property(
            "device", &CuImage::device, nullptr, doc::CuImage::doc_device, py::call_guard<py::gil_scoped_release>()) //
        .def_property("raw_metadata", &CuImage::raw_metadata, nullptr, doc::CuImage::doc_raw_metadata,
                      py::call_guard<py::gil_scoped_release>()) //
        .def_property(
            "metadata", &py_metadata, nullptr, doc::CuImage::doc_metadata, py::call_guard<py::gil_scoped_release>()) //
        .def_property("ndim", &CuImage::ndim, nullptr, doc::CuImage::doc_ndim, py::call_guard<py::gil_scoped_release>()) //
        .def_property("dims", &CuImage::dims, nullptr, doc::CuImage::doc_dims, py::call_guard<py::gil_scoped_release>()) //
        .def_property(
            "shape", &CuImage::shape, nullptr, doc::CuImage::doc_shape, py::call_guard<py::gil_scoped_release>()) //
        .def("size", &CuImage::size, doc::CuImage::doc_size, py::call_guard<py::gil_scoped_release>(), //
             py::arg("dim_order") = "" //
             ) //
        .def_property(
            "dtype", &CuImage::dtype, nullptr, doc::CuImage::doc_dtype, py::call_guard<py::gil_scoped_release>()) //
        .def_property("channel_names", &CuImage::channel_names, nullptr, doc::CuImage::doc_channel_names,
                      py::call_guard<py::gil_scoped_release>()) //
        .def("spacing", &CuImage::spacing, doc::CuImage::doc_spacing, py::call_guard<py::gil_scoped_release>(), //
             py::arg("dim_order") = "" //
             ) //
        .def("spacing_units", &CuImage::spacing_units, doc::CuImage::doc_spacing_units,
             py::call_guard<py::gil_scoped_release>(), //
             py::arg("dim_order") = "" //
             ) //
        .def_property(
            "origin", &CuImage::origin, nullptr, doc::CuImage::doc_origin, py::call_guard<py::gil_scoped_release>()) //
        .def_property("direction", &CuImage::direction, nullptr, doc::CuImage::doc_direction,
                      py::call_guard<py::gil_scoped_release>()) //
        .def_property("coord_sys", &CuImage::coord_sys, nullptr, doc::CuImage::doc_coord_sys,
                      py::call_guard<py::gil_scoped_release>()) //
        .def_property("resolutions", &py_resolutions, nullptr, doc::CuImage::doc_resolutions,
                      py::call_guard<py::gil_scoped_release>()) //
        .def("read_region", &py_read_region, doc::CuImage::doc_read_region, py::call_guard<py::gil_scoped_release>(), //
             py::arg("location") = py::list{}, //
             py::arg("size") = py::list{}, //
             py::arg("level") = 0, //
             py::arg("device") = io::Device(), //
             py::arg("buf") = py::none(), //
             py::arg("shm_name") = "") //
        .def_property("associated_images", &CuImage::associated_images, nullptr, doc::CuImage::doc_associated_images,
                      py::call_guard<py::gil_scoped_release>()) //
        .def("associated_image", &CuImage::associated_image, doc::CuImage::doc_associated_image,
             py::call_guard<py::gil_scoped_release>(), //
             py::arg("name") = "") //
        .def("save", &CuImage::save, doc::CuImage::doc_save, py::call_guard<py::gil_scoped_release>()) //
        .def("__bool__", &CuImage::operator bool, py::call_guard<py::gil_scoped_release>()) //
        .def(
            "__repr__", //
            [](const CuImage& cuimg) { //
                return fmt::format("<cucim.CuImage path:{}>", cuimg.path());
            },
            py::call_guard<py::gil_scoped_release>())
        .def_property("__array_interface__", &get_array_interface, nullptr, doc::CuImage::doc_get_array_interface,
                      py::call_guard<py::gil_scoped_release>());

    // We can use `"cpu"` instead of `Device("cpu")`
    py::implicitly_convertible<const char*, io::Device>();
}

std::string get_plugin_root()
{
    return CuImage::get_framework()->get_plugin_root();
}

void set_plugin_root(std::string path)
{
    CuImage::get_framework()->set_plugin_root(path.c_str());
}

template <typename PT, typename T>
pybind11::tuple vector2pytuple(const std::vector<T>& vec)
{
    py::tuple result(vec.size());
    std::vector<pybind11::object> args;
    args.reserve(vec.size());
    for (auto& arg_value : vec)
    {
        args.emplace_back(pybind11::reinterpret_steal<pybind11::object>(pybind11::detail::make_caster<PT>::cast(
            std::forward<PT>(arg_value), pybind11::return_value_policy::automatic_reference, nullptr)));
    }
    int counter = 0;
    for (auto& arg_value : args)
    {
        PyTuple_SET_ITEM(result.ptr(), counter++, arg_value.release().ptr());
    }
    return result;
}

json py_metadata(const CuImage& cuimg)
{
    auto metadata = cuimg.metadata();
    auto json_obj = json::parse(metadata.empty() ? "{}" : metadata);

    // Append basic metadata for the image
    auto item_iter = json_obj.emplace("cucim", json::object());
    json& cucim_metadata = *(item_iter.first);

    cucim_metadata.emplace("path", cuimg.path());
    cucim_metadata.emplace("ndim", cuimg.ndim());
    cucim_metadata.emplace("dims", cuimg.dims());
    cucim_metadata.emplace("shape", cuimg.shape());
    {
        const auto& dtype = cuimg.dtype();
        cucim_metadata.emplace(
            "dtype", json::object({ { "code", dtype.code }, { "bits", dtype.bits }, { "lanes", dtype.lanes } }));
    }
    cucim_metadata.emplace("channel_names", cuimg.channel_names());
    cucim_metadata.emplace("spacing", cuimg.spacing());
    cucim_metadata.emplace("spacing_units", cuimg.spacing_units());
    cucim_metadata.emplace("origin", cuimg.origin());
    cucim_metadata.emplace("direction", cuimg.direction());
    cucim_metadata.emplace("coord_sys", cuimg.coord_sys());
    {
        const auto& resolutions = cuimg.resolutions();
        auto resolutions_iter = cucim_metadata.emplace("resolutions", json::object());
        json& resolutions_metadata = *(resolutions_iter.first);
        auto level_count = resolutions.level_count();
        resolutions_metadata.emplace("level_count", level_count);
        std::vector<std::vector<int64_t>> level_dimensions_vec;
        level_dimensions_vec.reserve(level_count);
        for (int level = 0; level < level_count; ++level)
        {
            level_dimensions_vec.emplace_back(resolutions.level_dimension(level));
        }
        resolutions_metadata.emplace("level_dimensions", level_dimensions_vec);
        resolutions_metadata.emplace("level_downsamples", resolutions.level_downsamples());
    }
    cucim_metadata.emplace("associated_images", cuimg.associated_images());
    return json_obj;
}

py::dict py_resolutions(const CuImage& cuimg)
{
    const auto& resolutions = cuimg.resolutions();
    auto level_count = resolutions.level_count();
    if (resolutions.level_count() == 0)
    {
        return py::dict{
            "level_count"_a = pybind11::int_(0), //
            "level_dimensions"_a = pybind11::tuple(), //
            "level_downsamples"_a = pybind11::tuple() //
        };
    }

    std::vector<py::tuple> level_dimensions_vec;
    level_dimensions_vec.reserve(level_count);
    for (int level = 0; level < level_count; ++level)
    {
        level_dimensions_vec.emplace_back(vector2pytuple<pybind11::int_>(resolutions.level_dimension(level)));
    }

    py::tuple level_dimensions = vector2pytuple<const pybind11::tuple&>(level_dimensions_vec);
    py::tuple level_downsamples = vector2pytuple<pybind11::float_>(resolutions.level_downsamples());

    return py::dict{
        "level_count"_a = pybind11::int_(level_count), //
        "level_dimensions"_a = level_dimensions, //
        "level_downsamples"_a = level_downsamples //
    };
}


CuImage py_read_region(CuImage& cuimg,
                       std::vector<int64_t> location,
                       std::vector<int64_t> size,
                       int16_t level,
                       io::Device device,
                       py::object buf,
                       const std::string& shm_name,
                       py::kwargs kwargs)
{
    cucim::DimIndices indices;
    if (kwargs)
    {
        std::vector<std::pair<char, int64_t>> indices_args;

        for (auto item : kwargs)
        {
            auto key = std::string(py::str(item.first));
            auto value = py::cast<int>(item.second);

            if (key.size() != 1)
            {
                throw std::invalid_argument(
                    fmt::format("Argument name for Dimension should be a single character but '{}' is used.", key));
            }
            char key_char = key[0] & ~32;
            if (key_char < 'A' || key_char > 'Z')
            {
                throw std::invalid_argument(
                    fmt::format("Dimension character should be an alphabet but '{}' is used.", key));
            }

            indices_args.emplace_back(std::make_pair(key_char, value));

            //            fmt::print("k:{} v:{}\n", std::string(py::str(item.first)),
            //            std::string(py::str(item.second)));
        }
        indices = cucim::DimIndices(indices_args);
    }
    else
    {
        indices = cucim::DimIndices{};
    }
    cucim::CuImage region = cuimg.read_region(location, size, level, indices, device, nullptr, "");
    return region;
}

py::dict get_array_interface(const CuImage& cuimg)
{
    // TODO: using __array_struct__, access to array interface could be faster
    //       (https://numpy.org/doc/stable/reference/arrays.interface.html#c-struct-access)
    // TODO: check the performance difference between python int vs python long later.
    const DLTensor* tensor = static_cast<DLTensor*>(cuimg.container());
    if (!tensor)
    {
        return pybind11::dict();
    }
    const char* type_str = cuimg.container().numpy_dtype();

    py::list descr;
    descr.append(py::make_tuple(""_s, py::str(type_str)));

    py::tuple shape = vector2pytuple<pybind11::int_>(cuimg.shape());

    // Reference: https://numpy.org/doc/stable/reference/arrays.interface.html
    return py::dict{ "data"_a =
                         pybind11::make_tuple(py::int_(reinterpret_cast<uint64_t>(tensor->data)), py::bool_(false)),
                     "strides"_a = py::none(),
                     "descr"_a = descr,
                     "typestr"_a = py::str(type_str),
                     "shape"_a = shape,
                     "version"_a = py::int_(3) };
}


} // namespace cucim
