#include "midiplayer.hpp"
#include "synthesizer.hpp"
#include <QApplication>
#include <cstdlib>

#include <iostream>

int main(int argc, char ** argv) {

	
	QApplication app(argc, argv);
	MIDIPlayer a;
	a.show();        // returns immediately
	return app.exec();   // enters event loop


	return EXIT_SUCCESS;
}
