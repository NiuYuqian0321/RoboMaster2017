LDFLAGS = -Wl,-rpath,./ -L./ -lpthread -lrt -lv4l2 $(OPENCV_LINK)

OPENCV_FLAG = `pkg-config opencv --cflags`
OPENCV_LINK = `pkg-config opencv --libs` `pkg-config cuda-6.5 --libs` `pkg-config cudart-6.5 --libs`

./Release/Run.out : ./Release/Armor.o ./Release/Videolog.o ./Release/CConfig.o ./Release/Chariot.o ./Release/ChariotEx.o ./Release/Main.o 
	g++ -o ./Release/Run.out  ./Release/Armor.o ./Release/Videolog.o ./Release/CConfig.o ./Release/Chariot.o ./Release/ChariotEx.o ./Release/Main.o $(LDFLAGS)

./Release/Main.o : main.cpp CArmorDetection.h ChariotEx.h
	g++ -o ./Release/Main.o -c main.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)

./Release/Armor.o : CArmorDetection.cpp CArmorDetection.h
	g++ -o ./Release/Armor.o -c CArmorDetection.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)

./Release/ChariotEx.o : ChariotEx.cpp ChariotEx.h
	g++ -o ./Release/ChariotEx.o -c ChariotEx.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)

./Release/Chariot.o : Chariot.cpp Chariot.h CConfig.h
	g++ -o ./Release/Chariot.o -c Chariot.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)

./Release/Videolog.o : Videolog.cpp Videolog.h Mode.h
	g++ -o ./Release/Videolog.o -c Videolog.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)

./Release/CConfig.o : CConfig.cpp CConfig.h
	g++ -o ./Release/CConfig.o -c CConfig.cpp -g -Wall --std=c++11 $(OPENCV_FLAG)
	
clean:
	rm -f ./Release/*
	rm -f *~
