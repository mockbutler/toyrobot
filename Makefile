CXXFLAGS += -std=c++17 -Wall -Wextra -Werror

OBJ := toyrobot.o
EXE := toyrobot

$(EXE): $(OBJ)
	$(CXX) -o $@ $+

test: $(EXE)
	bash test.bash $(EXE)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ)

