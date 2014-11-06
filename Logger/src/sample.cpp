#include "logger.h"

int main(void) {

	std::cout << "デフォルト名でログを作成する" << std::endl;
	Logger *l = new Logger();
	
	l->write("test,");
	l->write(100);
	l->write(",");
	l->write(200.0f);
	l->write(",");
	l->write(300.0);
	l->write("\n");

	delete l;


	std::cout << "ファイル名(test)でログを作成する" << std::endl;
	l = new Logger("test");

	*l << "test," << 100 << "," << 200.0f  << "," << 300.0 << "\n";

	delete l;
}