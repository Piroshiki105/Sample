#include "logger.h"

int main(void) {

	Logger l;

	l.open("test.txt");

	l.write("test");

	l.close();

}