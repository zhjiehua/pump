#include "buglecompensation.h"
#include "BugleCompensationWithPID.h"
#include "buglecompensationwithsection.h"

//#define BUGLE_COMPENSATION_WITH_PID


BugleCompensation::BugleCompensation(QObject *parent)
	: QObject(parent)
	, m_nCurrentOutput(0)
	, m_bBugleSignal(false)
	,m_bOutputStable(false)
{

}

BugleCompensation::~BugleCompensation()
{

}

BugleCompensation * BugleCompensation::getInstance()
{
	static BugleCompensation *pBugleCompensation = 0;
	if( !pBugleCompensation )
	{
#ifdef BUGLE_COMPENSATION_WITH_PID
		pBugleCompensation = new BugleCompensationWithPID();
#else
		pBugleCompensation = new BugleCompensationWithSection();
#endif
	}
	return pBugleCompensation;
}



