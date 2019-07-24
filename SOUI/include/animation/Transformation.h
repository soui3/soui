/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <interface/render-i.h>
#include <matrix/SMatrix.h>

namespace SOUI
{
/**
 * Defines the transformation to be applied at
 * one point in time of an Animation.
 *
 */
class SOUI_EXP STransformation {
public:
enum{
    /**
     * Indicates a transformation that has no effect (alpha = 1 and identity matrix.)
     */
    TYPE_IDENTITY = 0x0,
    /**
     * Indicates a transformation that applies an alpha only (uses an identity matrix.)
     */
    TYPE_ALPHA = 0x1,
    /**
     * Indicates a transformation that applies a matrix only (alpha = 1.)
     */
    TYPE_MATRIX = 0x2,
    /**
     * Indicates a transformation that applies an alpha and a matrix.
     */
    TYPE_BOTH = TYPE_ALPHA | TYPE_MATRIX,
};

protected:
	SMatrix  mMatrix;
    BYTE	 mAlpha;
    int mTransformationType;

    /**
     * Creates a new transformation with alpha = 1 and the identity matrix.
     */
public:
	STransformation() {
        clear();
    }

    /**
     * Reset the transformation to a state that leaves the object
     * being animated in an unmodified state. The transformation type is
     * {@link #TYPE_BOTH} by default.
     */
    void clear() {
		mMatrix.reset();
        mAlpha = 255;
        mTransformationType = TYPE_IDENTITY;
    }

    /**
     * Indicates the nature of this transformation.
     *
     * @return {@link #TYPE_ALPHA}, {@link #TYPE_MATRIX},
     *         {@link #TYPE_BOTH} or {@link #TYPE_IDENTITY}.
     */
    int getTransformationType() const{
        return mTransformationType;
    }

	void setTransformationType(int type)
	{
		mTransformationType = type;
	}
    /**
     * Clones the specified transformation.
     *
     * @param t The transformation to clone.
     */
    void set(STransformation t) {
        mAlpha = t.getAlpha();
		mMatrix.SetData(t.getMatrix().GetData());

        mTransformationType = t.getTransformationType();
    }

    /**
     * Apply this STransformation to an existing STransformation, e.g. apply
     * a scale effect to something that has already been rotated.
     * @param t
     */
    void compose(STransformation t) {
		if (t.hasAlpha()) mAlpha = (BYTE)((int)mAlpha * t.getAlpha()/255);
		if (t.hasMatrix()) mMatrix = t.getMatrix() * mMatrix;
		updateType();
	}
    
    /**
     * Like {@link #compose(STransformation)} but does this.postConcat(t) of
     * the transformation matrix.
     * @hide
     */
    void postCompose(STransformation t) {
        if(t.hasAlpha()) mAlpha = (BYTE)((int)mAlpha*t.getAlpha()/255);
        if(t.hasMatrix()) mMatrix *= t.getMatrix();
		updateType();
    }

    /**
     * @return The 3x3 Matrix representing the trnasformation to apply to the
     * coordinates of the object being animated
     */
    const SMatrix & getMatrix() const {
        return mMatrix;
    }
    
	SMatrix & getMatrix() {
		return mMatrix;
	}

	void setMatrix(const SMatrix & mtx)
	{
		mMatrix = mtx;
		if (mMatrix.isIdentity())
			mTransformationType |= TYPE_MATRIX;
		else
			mTransformationType &= ~TYPE_MATRIX;
	}
    /**
     * Sets the degree of transparency
     * @param alpha 255 means fully opaqe and 0 means fully transparent
     */
    void setAlpha(BYTE alpha) {
        mAlpha = alpha;
		if (mAlpha == 0xFF)
			mTransformationType &= ~TYPE_ALPHA;
		else
			mTransformationType |= TYPE_ALPHA;
    }

	void updateType()
	{
		mTransformationType = TYPE_IDENTITY;
		if (mAlpha != 0xFF)
			mTransformationType |= TYPE_ALPHA;
		if (!mMatrix.isIdentity())
			mTransformationType |= TYPE_MATRIX;
	}

    /**
     * @return The degree of transparency
     */
    BYTE getAlpha() const{
        return mAlpha;
    }


	bool hasAlpha() const
	{
		return (getTransformationType() & TYPE_ALPHA) == TYPE_ALPHA;
	}

	bool hasMatrix() const
	{
		return (getTransformationType() & TYPE_MATRIX) == TYPE_MATRIX;
	}

	bool isIdentity() const
	{
		return getTransformationType() == TYPE_IDENTITY;
	}
};

}