
#include <souistd.h>
#include <animation/Transformation.h>

namespace SOUI {



	bool STransformation::isIdentity() const
	{
		return getTransformationType() == TYPE_IDENTITY;
	}

	bool STransformation::hasMatrix() const
	{
		return (getTransformationType() & TYPE_MATRIX) == TYPE_MATRIX;
	}

	bool STransformation::hasAlpha() const
	{
		return (getTransformationType() & TYPE_ALPHA) == TYPE_ALPHA;
	}

	BYTE STransformation::getAlpha() const
	{
		return mAlpha;
	}

	void STransformation::updateType()
	{
		mTransformationType = TYPE_IDENTITY;
		if (mAlpha != 0xFF)
			mTransformationType |= TYPE_ALPHA;
		if (!mMatrix.isIdentity())
			mTransformationType |= TYPE_MATRIX;
	}

	void STransformation::setAlpha(BYTE alpha)
	{
		mAlpha = alpha;
		if (mAlpha == 0xFF)
			mTransformationType &= ~TYPE_ALPHA;
		else
			mTransformationType |= TYPE_ALPHA;
	}

	void STransformation::setMatrix(const SMatrix & mtx)
	{
		mMatrix = mtx;
		if (!mMatrix.isIdentity())
			mTransformationType |= TYPE_MATRIX;
		else
			mTransformationType &= ~TYPE_MATRIX;
	}

	SMatrix & STransformation::getMatrix()
	{
		return mMatrix;
	}

	const SMatrix & STransformation::getMatrix() const
	{
		return mMatrix;
	}

	void STransformation::postCompose(STransformation t)
	{
		if(t.hasAlpha()) mAlpha = (BYTE)((int)mAlpha*t.getAlpha()/255);
		if(t.hasMatrix()) mMatrix *= t.getMatrix();
		updateType();
	}

	void STransformation::compose(STransformation t)
	{
		if (t.hasAlpha()) mAlpha = (BYTE)((int)mAlpha * t.getAlpha()/255);
		if (t.hasMatrix()) mMatrix = t.getMatrix() * mMatrix;
		updateType();
	}

	void STransformation::set(STransformation t)
	{
		mAlpha = t.getAlpha();
		mMatrix.SetData(t.getMatrix().GetData());

		mTransformationType = t.getTransformationType();
	}

	void STransformation::setTransformationType(int type)
	{
		mTransformationType = type;
	}

	int STransformation::getTransformationType() const
	{
		return mTransformationType;
	}

	void STransformation::clear()
	{
		mMatrix.reset();
		mAlpha = 255;
		mTransformationType = TYPE_IDENTITY;
	}

	STransformation::STransformation()
	{
		clear();
	}

}
