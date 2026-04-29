#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics.hpp>
#include <print>
#include <muParser.h>

struct FunctionData {
	std::string expression;
	mu::Parser parser;
	sf::Color color;
	std::vector<std::pair<float, float>> graph;
};

struct globals {
	static inline double step;
	static inline std::unordered_map<std::string, FunctionData> functions;
	static inline bool waitingInput;
};

sf::Color colorFromString(const std::string& name) {
    static const std::unordered_map<std::string, sf::Color> table = {
        {"red",     sf::Color::Red},
        {"green",   sf::Color::Green},
        {"blue",    sf::Color::Blue},
        {"white",   sf::Color::White},
        {"black",   sf::Color::Black},
        {"yellow",  sf::Color::Yellow},
        {"magenta", sf::Color::Magenta},
        {"cyan",    sf::Color::Cyan},
    };

    auto it = table.find(name);
    if (it != table.end()) return it->second;

    if (!name.empty() && name[0] == '#') {
        unsigned val = std::stoul(name.substr(1), nullptr, 16);
        if (name.size() == 7)
            return sf::Color((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
        if (name.size() == 9)
            return sf::Color((val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }

    return sf::Color::Transparent;
}

void createGraph(FunctionData& data) {
	double x = -10.0;
	data.parser.DefineVar("x", &x);

	while (x <= 10.0f) {
		//float y = a * std::pow(x, 2) + b * x + c;
		float y = data.parser.Eval();
		data.graph.push_back({x, y});
		x+=globals::step;
	}
}

void refreshGraphs() {
	for (auto& [k, v] : globals::functions) {
		v.graph = {};
		createGraph(v);
	}
}

void addNewFunction(const std::string& name, const std::string& expression, const std::string& color) {
	mu::Parser parser;
	parser.SetExpr(expression);
	globals::functions.insert_or_assign(name, FunctionData{
		.expression = expression,
		.parser = parser,
		.color = colorFromString(color)
	});
}

void update(sf::RenderWindow& window) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::Q) {
				window.setVisible(false);
				std::string name;
				std::string expr;
				std::string color;

				std::print("Enter function name: ");
				std::cin >> name;
				std::print("Graph color: ");
				std::cin >> color;
				std::print("{}(x) = ", name);
				std::cin >> expr;
				try {
					addNewFunction(name, expr, color);
					createGraph(globals::functions.at(name));
				} catch (mu::Parser::exception_type& e) {
					std::println("Err: {}", e.GetMsg());
				}
				window.setVisible(true);
				refreshGraphs();
				continue;
			} else if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
				window.setVisible(false);
				std::print("Step: ");
				std::cin >> globals::step;
				window.setVisible(true);
				refreshGraphs();
				continue;
			}
		}
    }

    window.clear(sf::Color::White);

	// draw axes

	for (float i = -10; i < 10; i++) {
		sf::VertexArray xaxis(sf::PrimitiveType::Lines, 2);
		xaxis[0].position = {i, -10};
		xaxis[1].position = {i, 10};
		xaxis[0].color = sf::Color(0, 0, 0, 64);
		xaxis[1].color = sf::Color(0, 0, 0, 64);

		sf::VertexArray yaxis(sf::PrimitiveType::Lines, 2);
		yaxis[0].position = {-10, i};
		yaxis[1].position = {10, i};
		yaxis[0].color = sf::Color(0, 0, 0, 64);
		yaxis[1].color = sf::Color(0, 0, 0, 64);

		window.draw(xaxis);
		window.draw(yaxis);
	}

	sf::VertexArray xaxis(sf::PrimitiveType::Lines, 2);
	xaxis[0].position = {0, 10};
	xaxis[1].position = {0, -10};
	xaxis[0].color = sf::Color::Black;
	xaxis[1].color = sf::Color::Black;

	sf::VertexArray yaxis(sf::PrimitiveType::Lines, 2);
	yaxis[0].position = {10, 0};
	yaxis[1].position = {-10, 0};
	yaxis[0].color = sf::Color::Black;
	yaxis[1].color = sf::Color::Black;

	for (const auto [k, v] : globals::functions) {
		sf::VertexArray graph(sf::PrimitiveType::Points);
		for (auto& point : v.graph) {
			sf::Vertex vertex;
			vertex.position = {point.first, point.second};
			vertex.color = v.color;
			graph.append(vertex);
		}
		window.draw(graph);
	}

	window.draw(xaxis);
	window.draw(yaxis);

    window.display();
}

int main(int argc, char* argv[]) {

	std::println("-----WELCOME TO FUNCTION GRAPHING THING-----");
	std::println("Press [Q] to add a function.");
	std::println("Press [W] to change step.");
	std::println("The names of functions have no restriction, any name is valid.");
	std::print("Step: ");
	std::cin >> globals::step;

    sf::RenderWindow window(sf::VideoMode({600, 600}), "Function graphing");
	window.setKeyRepeatEnabled(false);

    sf::View view(sf::FloatRect({-10, 10}, {20, -20}));
    window.setView(view);

    while (window.isOpen()) {
		update(window);
    }
}