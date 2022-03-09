#ifndef sem_assistant_h
#define sem_assistant_h
#include <QObject>
#include <QSemaphore>



/// ----------------------------------------------------------------------------
/// @brief: 信号量助手
/// ----------------------------------------------------------------------------
class SemAssistant 
{
public:
	explicit SemAssistant(QSemaphore& sem);
	~SemAssistant();


private:
	QSemaphore& sem_;

};



#endif /// sem_assistant_h