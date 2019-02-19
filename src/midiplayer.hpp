#ifndef MIDIPlayer_HPP
#define MIDIPlayer_HPP

// define class MIDIPlayer here

#include<QWidget>
#include<QLineEdit>
#include<string>
#include<QPushButton>
#include <QAudioOutput>
#include<QSlider>
#include "synthesizer.hpp"
#include <QDebug>
#include <thread>
#include "signal.hpp"
#include <fstream>

class QGroupBox;
class QFileDialog;
class QPushButton;
class QSlider;
class MIDIPlayer : public QWidget {
	Q_OBJECT

public:
	MIDIPlayer();
	~MIDIPlayer();
	QString getInputfile();
	void testOutput(const std::string &inputFile, const std::string &outputFile);
	void setState(int i);
	double getVolume();
	QueueType testQueue();

	public slots:
	void sendTerminate();
	void sendInput();

	private slots:
	void browse();
	void sendPlay();
	void sendPause();
	void sendStop();
	void volumeMute();
	void volumeChange();
	void handleNotify();

	
	
private:
	bool write_wav(Signal & sound, const std::string &filename);
	QLineEdit* inputpath_;
	QPushButton * Browse;
	QPushButton * Play;
	QPushButton * Pause;
	QPushButton * Stop;
	QPushButton * Mute;
	QSlider * volume_;
	message_queue *mq_;
	double_buffer *db_;
	std::thread *SynthThread;
	
	int state; //1 is play, 2 is paused, 3 is stopped, 4 is parsed, 5 is testmode
	std::size_t buffer_size;
	std::vector<char> buffer;
	QAudioOutput *audio;
	QIODevice *device;
};



#endif
