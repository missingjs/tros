SOURCES := $(shell find . -type f -name '*.cpp')
OBJECTS := $(SOURCES:.cpp=.o)
CXX := g++
CPPFLAGS := -I.
TARGET := main.out

all: $(TARGET)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) $(CPPFLAGS) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sinclude $(SOURCES:.cpp=.d)

$(TARGET): $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET) $(SOURCES:.cpp=.d)
