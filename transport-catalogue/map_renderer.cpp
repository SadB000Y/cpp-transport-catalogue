#include "map_renderer.h"
#include <set>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace transport_catalogue
{
    bool IsZero(double value)
    {
        return std::abs(value) < EPSILON;
    }
    svg::Color ParseColor(json::Node node)
    {
        svg::Color result;
        if (node.IsString())
        {
            result = node.AsString();
        }
        else if (node.AsArray().size() == 3)
        {
            result = svg::Rgb{ static_cast<uint8_t>(node.AsArray()[0].AsInt()),
                              static_cast<uint8_t>(node.AsArray()[1].AsInt()),
                              static_cast<uint8_t>(node.AsArray()[2].AsInt()) };
        }
        else
        {

            result = svg::Rgba{ static_cast<uint8_t>(node.AsArray()[0].AsInt()),
                               static_cast<uint8_t>(node.AsArray()[1].AsInt()),
                               static_cast<uint8_t>(node.AsArray()[2].AsInt()),
                               node.AsArray()[3].AsDouble() };
        }
        return result;
    }

    SvgInfo ParsePropLine(json::Node node)
    {
        SvgInfo res;
        res.width = node.AsDict().at("width").AsDouble();
        res.height = node.AsDict().at("height").AsDouble();
        res.padding = node.AsDict().at("padding").AsDouble();
        res.line_width = node.AsDict().at("line_width").AsDouble();
        res.stop_radius = node.AsDict().at("stop_radius").AsDouble();
        res.bus_label_font_size = node.AsDict().at("bus_label_font_size").AsInt();
        res.bus_label_offset = { node.AsDict().at("bus_label_offset").AsArray()[0].AsDouble(),
                                node.AsDict().at("bus_label_offset").AsArray()[1].AsDouble() };

        res.stop_label_font_size = node.AsDict().at("stop_label_font_size").AsInt();
        res.stop_label_offset = { node.AsDict().at("stop_label_offset").AsArray()[0].AsDouble(),
                                 node.AsDict().at("stop_label_offset").AsArray()[1].AsDouble() };
        res.underlayer_color = ParseColor(node.AsDict().at("underlayer_color"));
        res.underlayer_width = node.AsDict().at("underlayer_width").AsDouble();

        auto color_array = node.AsDict().at("color_palette").AsArray();
        for (auto memb : color_array)
        {
            res.color_palette.push_back(ParseColor(memb));
        }
        return res;
    }

    svg::Document& FormSVGDocument(TransportCatalogue& catal, SvgInfo& prop)
    {
        static svg::Document result;
        auto all_buses = catal.GetAllBuses();

        // Формирование вектора всех остановок для передачи конструктору SphereProjector
        std::vector<geo::Coordinates> coords_of_all_stops_buses;
        for (auto memb : all_buses)
        {
            for (auto stop : memb.stops)
            {
                coords_of_all_stops_buses.push_back(stop->coordinates);
            }
        }

        // Процесс сортировки маршрутов по возрастанию названий
        std::vector<Buses> temp_vect;

        for (auto memb : all_buses)
        {
            temp_vect.push_back(memb);
        }
        std::sort(temp_vect.begin(), temp_vect.end(), [](Buses& lhs, Buses& rhs)
            { return lhs.name < rhs.name; });

        int color_iterator = 0;
        for (auto& memb : temp_vect)
        {
            result.Add(std::move(DrawThePolyline(coords_of_all_stops_buses, memb, prop, color_iterator)));
            
        }
        color_iterator = 0;
        for (auto& memb : temp_vect)
        {
            std::vector<svg::Text> result_names_routes = std::move(DrawRouteNames(coords_of_all_stops_buses, memb, prop, color_iterator));
            if (result_names_routes.size() != 0)
            {
                for (auto& memb : result_names_routes)
                {
                    result.Add(std::move(memb));
                }
            }
        }

        DrawStops(coords_of_all_stops_buses, catal, prop, result);

        DrawStopNames(coords_of_all_stops_buses, catal, prop, result);
        return result;
    }

    void DrawStops(std::vector<geo::Coordinates>& coords, TransportCatalogue& catal, SvgInfo& prop, svg::Document& result_doc)
    {
        std::vector<svg::Circle> result;
        auto projectorinfo = SphereProjector(coords.begin(), coords.end(), prop.width, prop.height, prop.padding);

        auto all_unique_stops = catal.GetUniqueStops();
        result.reserve(all_unique_stops.size());
        for (auto memb : all_unique_stops)
        {
            geo::Coordinates coords_of_stop = catal.FindStop(std::string{ memb })->coordinates;
            svg::Point pt = projectorinfo(coords_of_stop);
            svg::Circle stop;
            stop.SetCenter(pt)
                .SetRadius(prop.stop_radius)
                .SetFillColor("white");
            result.emplace_back(std::move(stop));
        }

        for (auto& memb : result)
        {
            result_doc.Add(std::move(memb));
        }
    }

    void DrawStopNames(std::vector<geo::Coordinates>& coords, TransportCatalogue& catal, SvgInfo& prop, svg::Document& result_doc)
    {
        std::vector<svg::Text> result;
        auto projectorinfo = SphereProjector(coords.begin(), coords.end(), prop.width, prop.height, prop.padding);

        auto all_unique_stops = catal.GetUniqueStops();
        result.reserve(all_unique_stops.size());
        for (auto stop : all_unique_stops)
        {
            geo::Coordinates coords_of_stop = catal.FindStop(std::string{ stop })->coordinates;
            svg::Point pt = projectorinfo(coords_of_stop);

            svg::Text add_text;
            add_text.SetPosition(pt)
                .SetOffset({ prop.stop_label_offset.dx, prop.stop_label_offset.dy })
                .SetFontSize(prop.stop_label_font_size)
                .SetFontFamily("Verdana")
                .SetData(catal.FindStop(std::string{ stop })->name)
                .SetFillColor(prop.underlayer_color)
                .SetStrokeColor(prop.underlayer_color)
                .SetStrokeWidth(prop.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            result.emplace_back(std::move(add_text));

            svg::Text main_text;
            main_text.SetPosition(pt)
                .SetOffset({ prop.stop_label_offset.dx, prop.stop_label_offset.dy })
                .SetFontSize(prop.stop_label_font_size)
                .SetFontFamily("Verdana")
                .SetData(catal.FindStop(std::string{ stop })->name)
                .SetFillColor("black");

            result.emplace_back(std::move(main_text));
        }

        for (auto& memb : result)
        {
            result_doc.Add(std::move(memb));
        }
    }

    std::vector<svg::Text> DrawRouteNames(std::vector<geo::Coordinates>& coords, Buses bus, SvgInfo& prop, int& color_iterator)
    {
        if (bus.stops.size() != 0)
        {
            std::vector<svg::Text> result; //!!!!
            result.reserve(4);
            auto projectorinfo = SphereProjector(coords.begin(), coords.end(), prop.width, prop.height, prop.padding);

            svg::Point pt = projectorinfo(bus.stops[0]->coordinates);
            svg::Text add_name;
            add_name.SetPosition(pt)
                .SetOffset({ prop.bus_label_offset.dx, prop.bus_label_offset.dy })
                .SetFontSize(prop.bus_label_font_size)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(bus.name)
                .SetFillColor(prop.underlayer_color)
                .SetStrokeColor(prop.underlayer_color)
                .SetStrokeWidth(prop.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            result.emplace_back(std::move(add_name));

            svg::Text name;
            name.SetPosition(pt)
                .SetOffset({ prop.bus_label_offset.dx, prop.bus_label_offset.dy })
                .SetFontSize(prop.bus_label_font_size)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(bus.name)
                .SetFillColor(prop.color_palette[color_iterator % prop.color_palette.size()]);

            result.emplace_back(std::move(name));

            if (!bus.is_roundtrip)
            {
                svg::Point pt_2 = projectorinfo(bus.stops[(bus.stops.size() - 1) / 2]->coordinates);
                if ((pt_2.x != pt.x) && (pt_2.y != pt.y))
                {

                    svg::Text second_add_name;
                    second_add_name.SetPosition(pt_2)
                        .SetOffset({ prop.bus_label_offset.dx, prop.bus_label_offset.dy })
                        .SetFontSize(prop.bus_label_font_size)
                        .SetFontFamily("Verdana")
                        .SetFontWeight("bold")
                        .SetData(bus.name)
                        .SetFillColor(prop.underlayer_color)
                        .SetStrokeColor(prop.underlayer_color)
                        .SetStrokeWidth(prop.underlayer_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                    result.emplace_back(std::move(second_add_name));

                    svg::Text second_name;
                    second_name.SetPosition(pt_2)
                        .SetOffset({ prop.bus_label_offset.dx, prop.bus_label_offset.dy })
                        .SetFontSize(prop.bus_label_font_size)
                        .SetFontFamily("Verdana")
                        .SetFontWeight("bold")
                        .SetData(bus.name)
                        .SetFillColor(prop.color_palette[color_iterator % prop.color_palette.size()]);

                    result.emplace_back(std::move(second_name));
                }
            }
            color_iterator++;
            return result;
        }
        else
            color_iterator++;
        return {};
    }

    svg::Polyline DrawThePolyline(std::vector<geo::Coordinates>& coords, Buses bus, SvgInfo& prop, int& color_iterator)
    {
        svg::Polyline result;
        result.SetFillColor("none")
            .SetStrokeWidth(prop.line_width)
            .SetStrokeColor(prop.color_palette[color_iterator % prop.color_palette.size()])
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        auto projectorinfo = SphereProjector(coords.begin(), coords.end(), prop.width, prop.height, prop.padding);

        // Теперь строим точки
        for (auto stop : bus.stops)
        {
            svg::Point pt = projectorinfo(stop->coordinates);
            result.AddPoint(std::move(pt));
        }

        color_iterator++;
        return result;
    }

}