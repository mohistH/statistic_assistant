#include "SemAssistant.h"



/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
SemAssistant::SemAssistant(QSemaphore& sem)
	: sem_(sem)
{
	sem_.acquire();
}

SemAssistant::~SemAssistant()
{
	sem_.release();
}
