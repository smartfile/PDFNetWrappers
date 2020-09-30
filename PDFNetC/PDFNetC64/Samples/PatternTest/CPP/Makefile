TESTNAME=PatternTest
CXX=g++
TARGET=$(TESTNAME)
INCLUDES=-I../../../Headers
CXXFLAGS=
LIBS=-L../../../Lib -lPDFNetC -lstdc++ -lpthread -lm -lc
LDFLAGS=-Wl,-rpath,../../../Lib -Wl,-rpath,$$ORIGIN

$(TARGET): $(TARGET).cpp.o
	@echo "Linking target: $(TARGET)"
	$(CXX) -o $@ $< $(LIBS) $(LDFLAGS)

$(TARGET).cpp.o: $(TARGET).cpp
	@echo "Compiling target: $(TARGET)"
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

all: $(TARGET)

clean:
	rm -fr $(TARGET) *.o

rebuild: clean all

.PHONY: all clean rebuild
