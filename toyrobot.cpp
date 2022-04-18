#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>

/// Compass points used for direction.
enum Direction : int {
	North	= 0, 
	East	= 90, 
	South	= 180,
	West	= 270
};

/// Distance in degrees between compass points used
/// for direction.
constexpr int CompassPointSeparation = 90;

/// Maximum value for a compass point angle, also
/// equivalent to zero.
constexpr int CompassPointMax = 360;

/// Transform a direction to string.
std::string DirToName(Direction direction) {
	static const std::vector<std::string> DirectionNames = {
		// Must match order of enumeration.
		"NORTH", "EAST", "SOUTH", "WEST"
	};
	int index = (direction % CompassPointMax) / CompassPointSeparation;
	return DirectionNames[index];
}

/// Transform a string to direction value.
/// @exception std::invalid_argument Invalid direction name.
Direction NameToDir(const std::string &name) {
	static const std::map<std::string, Direction> mapping = {
		{ "NORTH", North },
		{ "EAST", East },
		{ "SOUTH", South },
		{ "WEST", West }
	};
	auto entry = mapping.find(name);
	if (entry == mapping.end()) {
		throw std::invalid_argument("Invalid direction.");
	}
	return entry->second;
};

/// Toy robot state. 
class ToyRobot {
	int x_;
	int y_;
	Direction facing_;

	friend std::ostream& operator<<(std::ostream&, const ToyRobot&);

	/// Change direction by specified number of degrees.
	/// @param turnAngle Must be a multiple of 90 degrees.
	Direction Turn(int turnAngle) {
		assert(turnAngle % CompassPointSeparation == 0);
		int angle = static_cast<int>(facing_) + (turnAngle % CompassPointMax);
		return static_cast<Direction>((angle < 0) ? CompassPointMax + angle : angle);
	}

public:
	ToyRobot(int x, int y, Direction facing) :
		x_(x),
		y_(y),
		facing_(facing)
	{}

	/// @return X coordinate.
	int X() const { return x_; }

	/// @return Y coordinate.
	int Y() const { return y_; }

	/// @return Current direction the robot is facing.
	Direction Facing() const {
		return facing_;
	}

	/// Change direction 90 degrees to the left.
	void TurnLeft() {
		facing_ = Turn(-90);
	}

	/// Change direction 90 degrees to the right.
	void TurnRight() {
		facing_ = Turn(90);
	}

	/// Move the robot 1 unit forward in the current direction.
	/// @return Previous robot state.
	ToyRobot Move() {
		ToyRobot previous{ *this };
		switch (facing_) {
			case North: y_++; break;
			case East:	x_++; break;
			case South:	y_--; break;
			case West:	x_--; break;
		}
		return previous;
	}

	/// Place the robot on the table.
	/// @return Previous robot state. 
	ToyRobot Place(int x, int y, Direction dir) {
		ToyRobot previous{ *this };
		x_ = x;
		y_ = y;
		facing_ = dir;
		return previous;
	}
};

std::ostream& operator<<(std::ostream& out, const ToyRobot& robot)
{
	return out << robot.x_ << ',' << robot.y_ << ',' << DirToName(robot.facing_);
}

/// Separate line into tokens based separated by whitespace and
/// comma characters.
///
/// @return Tokens in order from left to right.
std::vector<std::string> Tokenize(std::string& line) {

	// @return True if character is used to separate tokens.
	auto Separator = [](unsigned char c){ 
		return std::isspace(c) || c == ','; 
	};
	
	// Normalize text to uppercase.
	std::transform(line.begin(), line.end(), line.begin(), 
		[](char c){ return std::toupper(c); });

	std::vector<std::string> tokens;
	auto ch = line.begin();
	while (ch != line.end()) {
		// Skip non-token text.
		while (ch != line.end() && Separator(*ch)) { 
			ch++; 
		}
		std::string tok;
		while (ch != line.end() && !Separator(*ch)) {
			tok += *ch++;
		}
		if (tok.size() > 0) {
			tokens.push_back(tok);
		}
	}
	return tokens;
}

/// Read commands from an input stream where there is one
/// command per line.
///
/// @return Tokenized command text or an empty container
///		if command input is exhausted.
std::vector<std::string> ReadCommand(std::istream &input) {

	std::vector<std::string> tokens;
	std::string line;
	while (std::getline(input, line)) {
		tokens = Tokenize(line);
		if (!tokens.empty()) {
			break;
		}
	}
	return tokens;
}

int main(int argc, char **argv) {
	using namespace std;

	fstream input_file;
	if (argc > 1) {
		input_file.open(argv[1], ios::in);
		if (!input_file.is_open()) {
			std::cerr << "Error opening input: " << argv[1] << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	fstream output_file;
	if (argc > 2) {
		output_file.open(argv[2], ios::out|ios::trunc);
		if (!output_file.is_open()) {
			std::cerr << "Error opening output: " << argv[2] << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	auto& input = (input_file.is_open()) ? input_file : std::cin;
	auto& output = (output_file.is_open()) ? output_file : std::cout;

	/// Table size in units.
	const int TableSize = 5;
	
	/// Invalid X coordinate: guaranteed to be off table.
	constexpr int OffTableX = -1;

	/// Invalid Y coordinate: guaranteed to be off table.
	constexpr int OffTableY = -1;

	/// @return True if robot position is off the table.
	auto OffTable = [](const ToyRobot &robot) { 
		return robot.X() < 0 || robot.X() >= TableSize || 
			robot.Y() < 0 || robot.Y() >= TableSize;
	};

	// Robot starts "off" the table.
	ToyRobot robot{ OffTableX, OffTableY, North };

	for (;;) {
		auto cmdTokens = ReadCommand(input);
		if (cmdTokens.size() == 0) {
		 	break;
		}

		auto command = cmdTokens.front();
		// Ignore all commands other than "PLACE" until
		// the robot is on the table.
		if (OffTable(robot) && command != "PLACE") {
			continue;
		}

		auto args = cmdTokens.begin() + 1;
		auto argCount = cmdTokens.end() - args;

		if (argCount == 0 && command == "MOVE") {
			auto previous = robot.Move();
			if (OffTable(robot)) {
				// Ignore as robot moved off the table.
				robot = previous;
			}
		}
		else if (argCount == 0 && command == "LEFT") {
			robot.TurnLeft();
		}
		else if (argCount == 0 && command == "RIGHT") {
			robot.TurnRight();
		}
		else if (argCount == 0 && command == "REPORT") {
			output << robot << endl;
		}
		else if (argCount == 3 && command == "PLACE") {
			try {
				auto x = stoi(*args++);
				auto y = stoi(*args++);
				auto dir = NameToDir(*args++);
				auto previous = robot.Place(x, y, dir);
				if (OffTable(robot)) {
					// Ignore as it does not place the robot on the table.
					robot = previous;
				}
			} catch (...) {
				output << "One or more invalid arguments." << endl;
			}
		}
		else {
			output << "Unknown command or invalid argument count." << endl;
		}
	}

	exit(EXIT_SUCCESS);
}
