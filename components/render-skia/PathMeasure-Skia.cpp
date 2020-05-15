#include "stdafx.h"
#include "PathMeasure-Skia.h"
#include "render-skia.h"

namespace SOUI
{
	SPathMeasure_Skia::SPathMeasure_Skia(void)
	{
	}

	SPathMeasure_Skia::~SPathMeasure_Skia(void)
	{
	}

	void SPathMeasure_Skia::setPath(IPath * path, bool forceClosed)
	{
		SPath_Skia * skPath = (SPath_Skia*)path;
		m_pathMeasure.setPath(&skPath->m_skPath,forceClosed);
	}

	float SPathMeasure_Skia::getLength()
	{
		return m_pathMeasure.getLength();
	}

	BOOL SPathMeasure_Skia::getPosTan(float distance, float pos[], float tan[])
	{
		return !!m_pathMeasure.getPosTan(distance,(SkPoint*)pos,(SkVector*)tan);
	}

	BOOL SPathMeasure_Skia::getSegment(float startD, float stopD, IPath * dst, bool startWithMoveTo)
	{
		SPath_Skia * skPath = (SPath_Skia*)dst;
		return !!m_pathMeasure.getSegment(startD,stopD,&skPath->m_skPath,startWithMoveTo);
	}

}
