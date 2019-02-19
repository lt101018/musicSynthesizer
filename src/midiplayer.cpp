
// Implement class MIDIPlayer here

#include<QLabel>
#include<QFormLayout>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

#include "synthesizer.hpp"

#include<iostream>

#include "MIDIPlayer.hpp"

MIDIPlayer::MIDIPlayer() {
	mq_ = new message_queue;
	db_ = new double_buffer;
	SynthThread = new std::thread(synthesize, mq_, db_);
	state = 3; //1 is play, 2 is paused, 3 is stopped
	inputpath_ = new QLineEdit();
	inputpath_->setObjectName("inputpath");
	 Browse = new QPushButton("Browse");  //browse input file
	Browse->setObjectName("brows");
	 Play = new QPushButton("Play");  //browse input file
	Play->setObjectName("play");
	 Pause = new QPushButton("Pause");  //browse input file
	Pause->setObjectName("pause");
	 Stop = new QPushButton("Stop");  //browse input file
	Stop->setObjectName("stop");
	 Mute = new QPushButton("Mute");  //browse input file
	Mute->setObjectName("mute");

	volume_ = new QSlider(Qt::Horizontal);
	volume_->setObjectName("volume");
	volume_->setMaximum(100);
	volume_->setSliderPosition(50);

	QLabel* VolumeLabel = new QLabel("Volume:");

	QHBoxLayout* input = new QHBoxLayout();
	input->addWidget(inputpath_);
	input->addWidget(Browse);
	QGroupBox * inputgroup = new QGroupBox(tr("MIDI Input"));
	inputgroup->setLayout(input);

	QHBoxLayout* control1 = new QHBoxLayout();
	control1->addWidget(Play);
	control1->addWidget(Pause);
	control1->addWidget(Stop);
	QHBoxLayout* control2 = new QHBoxLayout();
	control2->addWidget(VolumeLabel);
	control2->addWidget(volume_);
	control2->addWidget(Mute);
	QVBoxLayout* control = new QVBoxLayout();
	control->addLayout(control1);
	control->addLayout(control2);

	QGroupBox * controlgroup = new QGroupBox(tr("Audio Controls"));
	controlgroup->setLayout(control);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(inputgroup);
	mainLayout->addWidget(controlgroup);
	setLayout(mainLayout);
	
	connect(volume_, SIGNAL(valueChanged(int)), this, SLOT(volumeChange()));
	connect(Browse, SIGNAL(clicked()), this, SLOT(browse()));
	connect(inputpath_, SIGNAL(returnPressed()), this, SLOT(sendInput()));
	connect(Play, SIGNAL(clicked()), this, SLOT(sendPlay()));
	connect(Pause, SIGNAL(clicked()), this, SLOT(sendPause()));
	connect(Stop, SIGNAL(clicked()), this, SLOT(sendStop()));
	connect(Mute, SIGNAL(clicked()), this, SLOT(volumeMute()));
	//need to revise after here:
	
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

	auto rates = info.supportedSampleRates();

	int sampleRate = rates.at(0);
	for (int i = 1; i < rates.size(); ++i) {
		if (rates.at(i) < sampleRate)
			sampleRate = rates.at(i);
	}

	QAudioFormat format;
	format.setSampleRate(sampleRate);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	if (!info.isFormatSupported(format)) {
		qWarning()<< "Raw audio format not supported by backend, cannot play audio.";
	}
	else {
		audio = new QAudioOutput(format);

		connect(audio, SIGNAL(notify()), this, SLOT(handleNotify()));
		buffer_size = audio->bufferSize();
		buffer_size = buffer_size > 0 ? buffer_size : 1024;

		audio->setNotifyInterval(25);

		audio->setVolume(0.5);
		
		
	}
	
}

MIDIPlayer::~MIDIPlayer() {
	//SynthThread->~thread();
	sendTerminate();
	SynthThread->join();

}

QueueType MIDIPlayer::testQueue() {
	QueueType a;
	mq_->wait_and_pop(a);
	return a;
}

void MIDIPlayer::sendTerminate() {
	QueueType a;
	a.flag = 5;
	mq_->push(a);
}

void MIDIPlayer::sendInput() {
	QString fileName = inputpath_->text();
	std::ifstream in(fileName.toStdString());
	if (!in && state != 5)  //check the avalibility of a file
	{
		QMessageBox::information(NULL, "Error", "The input file is invalid");
	}
	else {
		QueueType a;
		a.flag = 1;
		a.input = fileName.toStdString();
		mq_->push(a);
		state = 4;
	}
}

void MIDIPlayer::volumeMute() {
	volume_->setSliderPosition(0);
}

void MIDIPlayer::handleNotify() {

	int bytesFree = audio->bytesFree();
	int bytesToWrite = bytesFree > buffer_size ? buffer_size : bytesFree;
	int samplesToWrite = bytesToWrite >> 1;
	bytesToWrite = 2 * samplesToWrite;

	buffer.reserve(bytesToWrite);

	double s;
	for (int i = 0; i < samplesToWrite; ++i) {
		s = 0;
		db_->wait_and_pop(s);
		int16_t temp = static_cast<int16_t>(lround(s));
		char *byte1 = reinterpret_cast<char *>(&temp);
		char *byte2 = ++byte1;
		buffer.push_back(*byte1);
		buffer.push_back(*byte2);
	}

	if (bytesToWrite > 0) {
		device->write(buffer.data(), bytesToWrite);
	}

	buffer.clear();
}

QString MIDIPlayer::getInputfile() {
	return inputpath_->text();
	state = 4;
}


void MIDIPlayer::browse() {
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open MIDI file"), QDir::currentPath(), tr("MIDI Files (*.mid)"));
	inputpath_->setText(fileName);
	if(inputpath_->text().size()>0)
	{
		std::ifstream in(fileName.toStdString());
		if (!in && state != 5)  //check the avalibility of a file
		{
			QMessageBox::information(NULL, "Error", "The input file is invalid");
		}
		else {
			QueueType a;
			a.flag = 1;
			a.input = fileName.toStdString();
			mq_->push(a);
			state = 4;
		}
	}
	
}

void MIDIPlayer::setState(int i) {
	state = i;
}

void MIDIPlayer::sendPlay() {
	if (state == 2)
	{
		QueueType a;
		a.flag = 2;
		mq_->push(a);
		audio->resume();
		state = 1;
		Browse->setEnabled(false);
		Play->setEnabled(false);
	}
	else if (state == 4)
	{
		device = audio->start();
		QueueType a;
		a.flag = 2;
		mq_->push(a);
		state = 1;
		Browse->setEnabled(false);
		Play->setEnabled(false); 
		handleNotify();
	}
	else if (state == 5)
	{
		QueueType a;
		a.flag = 2;
		mq_->push(a);
	}
}

void MIDIPlayer::sendPause() {
	
	if (state == 1)
	{
		QueueType a;
		a.flag = 3;
		mq_->push(a);
		audio->suspend();
		state = 2;
		Play->setEnabled(true);
	}
	else if (state == 5)
	{
		QueueType a;
		a.flag = 3;
		mq_->push(a);
	}
}

void MIDIPlayer::sendStop() {
	
	if (state == 1 || state == 2)
	{
		QueueType a;
		a.flag = 4;
		mq_->push(a);
		audio->reset();
		buffer.clear();
		db_->clear();
		state = 4;
		Browse->setEnabled(true);
		Play->setEnabled(true);
	}
	else if (state == 5)
	{
		QueueType a;
		a.flag = 4;
		mq_->push(a);
	}
}

double MIDIPlayer::getVolume() {
	double x = volume_->value();
	return x;
}

void MIDIPlayer::volumeChange() {
	double x = volume_->value();
	audio->setVolume(x/100);
}

void MIDIPlayer::testOutput(const std::string &inputFile, const std::string &outputFile) {

	std::ifstream in(inputFile);
	if (!in)  //check the avalibility of a file
	{
		QMessageBox::information(NULL, "Error", "The input file is invalid");
	}
	else
	{
		Track track = readMIDIFromFile(inputFile).front();
		DefaultInstrument inst(track);
		inst.reset();
		Signal sig(44100);
		double deltaT = 1 / double(sig.getSampleRate());
		double value;
		while (!inst.halted())
		{
			value = inst.sample(deltaT);
			sig.push_back(value);
		}
		sig.normalize();
		write_wav(sig, outputFile);
	}
}

bool MIDIPlayer::write_wav(Signal & sound, const std::string &filename) {
	int8_t ChunkID[4] = { 'R','I','F','F' }, Format[4] = { 'W','A','V','E' }, Subchunk1ID[4] = { 'f','m','t',' ' }, Subchunk2ID[4] = { 'd','a','t','a' };
	int32_t Subchunk1Size = 16;
	int16_t AudioFormat = 1, NumChannels = 1, BitsPerSample = 16, BlockAlign = NumChannels*BitsPerSample / 8;
	int32_t ByteRate = 88200;
	int NumSamples = sound.size();
	int32_t  Subchunk2Size = NumSamples * NumChannels * BitsPerSample / 8, ChunkSize = 4 + (8 + Subchunk1Size) + (8 + Subchunk2Size), SampleRate = sound.getSampleRate();
	unsigned int i = 0; //the index of signal elements
	if (sound.size() == 0)
		return EXIT_FAILURE;
	std::string outputFile = filename + ".wav";
	std::ofstream out(outputFile, std::ios::binary);
	out.write(reinterpret_cast<std::fstream::char_type*>(&ChunkID), sizeof ChunkID);
	out.write(reinterpret_cast<std::fstream::char_type*>(&ChunkSize), sizeof ChunkSize);
	out.write(reinterpret_cast<std::fstream::char_type*>(&Format), sizeof Format);
	out.write(reinterpret_cast<std::fstream::char_type*>(&Subchunk1ID), sizeof Subchunk1ID);
	out.write(reinterpret_cast<std::fstream::char_type*>(&Subchunk1Size), sizeof Subchunk1Size);
	out.write(reinterpret_cast<std::fstream::char_type*>(&AudioFormat), sizeof AudioFormat);
	out.write(reinterpret_cast<std::fstream::char_type*>(&NumChannels), sizeof NumChannels);
	out.write(reinterpret_cast<std::fstream::char_type*>(&SampleRate), sizeof SampleRate);
	out.write(reinterpret_cast<std::fstream::char_type*>(&ByteRate), sizeof ByteRate);
	out.write(reinterpret_cast<std::fstream::char_type*>(&BlockAlign), sizeof BlockAlign);
	out.write(reinterpret_cast<std::fstream::char_type*>(&BitsPerSample), sizeof BitsPerSample);
	out.write(reinterpret_cast<std::fstream::char_type*>(&Subchunk2ID), sizeof Subchunk2ID);
	out.write(reinterpret_cast<std::fstream::char_type*>(&Subchunk2Size), sizeof Subchunk2Size);

	for (i = 0; i < sound.size(); i++)
	{
		int16_t data = sound[i];
		out.write(reinterpret_cast<std::fstream::char_type*>(&data), sizeof data);
	}

	return EXIT_SUCCESS;

}