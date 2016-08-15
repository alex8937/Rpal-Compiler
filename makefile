all:
	g++ -std=c++11 Token.cc Control_element.cc parser.cc rpal-parser.cc -o pl
cl:
	rm -f pl
