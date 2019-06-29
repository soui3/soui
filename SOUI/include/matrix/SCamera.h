/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

//  Inspired by Rob Johnson's most excellent QuickDraw GX sample code

#ifndef _SCamera_DEFINED_
#define _SCamera_DEFINED_

#include "SMatrix.h"

namespace SOUI {
	class S3DView;
	class SOUI_EXP SCamera  {
	public:
		SCamera();
		~SCamera();

		void save();
		void restore();

		void translate(float x, float y, float z);
		void rotateX(float deg);
		void rotateY(float deg);
		void rotateZ(float deg);

		void setCameraLocation(float x, float y, float z);
		float getCameraLocationX();
		float getCameraLocationY();
		float getCameraLocationZ();

		void getMatrix(SMatrix* out) const;

	private:
		SCamera(const SCamera&);
		SCamera& operator=(const SCamera&);

		S3DView * m_priv;
	};
}//end of namespace SOUI

#endif
