#include <QTest>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>

#include "midiplayer.hpp"
#include "test_config.hpp"

class TestMIDIPlayer : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();

  void input();

  void play();

  void pause();

  void stop();

  void volume();

  void mute();

  void synth();

  // TODO: add your tests
  
private:

  MIDIPlayer widget;
};

void TestMIDIPlayer::initTestCase(){

  {
    auto w = widget.findChild<QLineEdit *>("inputpath");
    QVERIFY2(w, "Could not find a QLineEdit widget");
  }

  {
    auto w = widget.findChild<QPushButton *>("play");
    QVERIFY2(w, "Could not find play button");
  }

  {
    auto w = widget.findChild<QPushButton *>("pause");
    QVERIFY2(w, "Could not find pause button");
  }
  
  {
    auto w = widget.findChild<QPushButton *>("stop");
    QVERIFY2(w, "Could not find stop button");
  }

  {
    auto w = widget.findChild<QSlider *>("volume");
    QVERIFY2(w, "Could not find volume slider");
  }
  
  {
    auto w = widget.findChild<QPushButton *>("mute");
    QVERIFY2(w, "Could not find mute button");
  }
}

void TestMIDIPlayer::input() {
	{
		widget.sendTerminate();
		auto w = widget.findChild<QLineEdit *>("inputpath");
		w->setText("testing path");
		QString text = widget.getInputfile();
		QVERIFY2(text == "testing path", "getInputfile() Function error");
		
		for (int i = 0; i < 50; i++)
		{
			widget.setState(5);
			widget.sendInput();
			QueueType a = widget.testQueue();
			QVERIFY2(a.flag == 1, "sendInput() Function error: flag is wrong");
			QVERIFY2(a.input == "testing path", "sendInput() Function error: input path is wrong");
		}
	}
}

void TestMIDIPlayer::play() {
	{
		//widget.sendTerminate();
		auto w = widget.findChild<QPushButton *>("play");
		widget.setState(5);
		for (int i = 0; i < 50; i++)
		{
			QTest::mouseClick(w, Qt::LeftButton);
			QueueType a = widget.testQueue();
			QVERIFY2(a.flag == 2, "sendPlay() Function error: flag is wrong");
		}
	}
}

void TestMIDIPlayer::pause() {
	{
		//widget.sendTerminate();
		auto w = widget.findChild<QPushButton *>("pause");
		widget.setState(5);
		for (int i = 0; i < 50; i++)
		{
			QTest::mouseClick(w, Qt::LeftButton);
			QueueType a = widget.testQueue();
			QVERIFY2(a.flag == 3, "sendPause() Function error: flag is wrong");
		}
	}
}

void TestMIDIPlayer::stop() {
	{
		//widget.sendTerminate();
		auto w = widget.findChild<QPushButton *>("stop");
		widget.setState(5);
		for (int i = 0; i < 50; i++)
		{
			QTest::mouseClick(w, Qt::LeftButton);
			QueueType a = widget.testQueue();
			QVERIFY2(a.flag == 4, "sendStop() Function error: flag is wrong");
		}
	}
}

void TestMIDIPlayer::volume() {
	{
		auto w = widget.findChild<QSlider *>("volume");
		for (int i = 0; i < 50; i++)
		{
			w->setSliderPosition(i);
			QVERIFY2(widget.getVolume() == i, "volumeChange() Function error");
		}
	}
}

void TestMIDIPlayer::mute() {
	{
		auto x = widget.findChild<QSlider *>("volume");
		auto w = widget.findChild<QPushButton *>("mute");
		for (int i = 0; i < 50; i++)
		{
			QTest::mouseClick(w, Qt::LeftButton);
			QVERIFY2(widget.getVolume() == 0, "volumeMute() Function error");
			x->setSliderPosition(i);
		}
	}
}

void TestMIDIPlayer::synth() {
	{
		QString indir = QString::fromStdString(TEST_FILE_DIR) + "/test";
		QString outdir = QString::fromStdString(TEST_FILE_DIR) + "/output";
		auto w = widget.findChild<QLineEdit *>("inputpath");
		
		for (int i = 0; i < 7; i++)
		{
			std::cout << "Test file " << i << ": ";
			QString input, output;
			input = indir + QString::number(i) + ".mid";
			output = outdir + QString::number(i);
			widget.testOutput(input.toStdString(), output.toStdString());
		}
	}
}

QTEST_MAIN(TestMIDIPlayer)
#include "test_midiplayer.moc"

