#include "logger.h"

int main(void) {

	std::cout << "�f�t�H���g���Ń��O���쐬����" << std::endl;
	Logger *l = new Logger();
	
	l->write("test,");
	l->write(100);
	l->write(",");
	l->write(200.0f);
	l->write(",");
	l->write(300.0);
	l->write("\n");

	delete l;


	std::cout << "�t�@�C����(test)�Ń��O���쐬����" << std::endl;
	l = new Logger("test");

	*l << "test," << 100 << "," << 200.0f  << "," << 300.0 << "\n";

	delete l;
}