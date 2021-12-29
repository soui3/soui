/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <souistd.h>
#include "matrix/SCamera.h"

namespace SOUI{

	/**
	*  Use to cast a pointer to a different type, and maintaining strict-aliasing
	*/
	template <typename Dst> Dst SkTCast(const void* ptr) {
		union {
			const void* src;
			Dst dst;
		} data;
		data.src = ptr;
		return data.dst;
	}


	struct SkUnit3D {
		float fX, fY, fZ;

		void set(float x, float y, float z) {
			fX = x; fY = y; fZ = z;
		}
		static float Dot(const SkUnit3D&, const SkUnit3D&);
		static void Cross(const SkUnit3D&, const SkUnit3D&, SkUnit3D* cross);
	};

	struct SkPoint3D {
		float    fX, fY, fZ;

		void set(float x, float y, float z) {
			fX = x; fY = y; fZ = z;
		}
		float    normalize(SkUnit3D*) const;
	};
	typedef SkPoint3D SkVector3D;

	struct SkMatrix3D {
		float    fMat[3][4];

		void reset();

		void setRow(int row, float a, float b, float c, float d = 0) {
			SASSERT((unsigned)row < 3);
			fMat[row][0] = a;
			fMat[row][1] = b;
			fMat[row][2] = c;
			fMat[row][3] = d;
		}

		void setRotateX(float deg);
		void setRotateY(float deg);
		void setRotateZ(float deg);
		void setTranslate(float x, float y, float z);

		void preRotateX(float deg);
		void preRotateY(float deg);
		void preRotateZ(float deg);
		void preTranslate(float x, float y, float z);

		void setConcat(const SkMatrix3D& a, const SkMatrix3D& b);
		void mapPoint(const SkPoint3D& src, SkPoint3D* dst) const;
		void mapVector(const SkVector3D& src, SkVector3D* dst) const;

		void mapPoint(SkPoint3D* v) const {
			this->mapPoint(*v, v);
		}

		void mapVector(SkVector3D* v) const {
			this->mapVector(*v, v);
		}
	};

	class SkPatch3D {
	public:
		SkPatch3D();

		void    reset();
		void    transform(const SkMatrix3D&, SkPatch3D* dst = NULL) const;

		// dot a unit vector with the patch's normal
		float dotWith(float dx, float dy, float dz) const;
		float dotWith(const SkVector3D& v) const {
			return this->dotWith(v.fX, v.fY, v.fZ);
		}

	public: // make public for SkDraw3D for now
		SkVector3D  fU, fV;
		SkPoint3D   fOrigin;

		friend class SkCamera3D;
	};

	class SkCamera3D {
	public:
		SkCamera3D();

		void reset();
		void update();
		void patchToMatrix(const SkPatch3D&, SMatrix* matrix) const;

		SkPoint3D   fLocation;
		SkPoint3D   fAxis;
		SkPoint3D   fZenith;
		SkPoint3D   fObserver;

	private:
		mutable SMatrix    fOrientation;
		mutable bool        fNeedToUpdate;

		void doUpdate() const;
	};

	class S3DView  {
	public:
		S3DView();
		~S3DView();

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

		void getMatrix(SMatrix*) const;

		float dotWithNormal(float dx, float dy, float dz) const;

	private:
		struct Rec {
			Rec*        fNext;
			SkMatrix3D  fMatrix;
		};
		Rec*        fRec;
		Rec         fInitialRec;
		SkCamera3D  fCamera;
	};


static float SFloatDotDiv(int count, const float a[], int step_a,
                               const float b[], int step_b,
                               float denom) {
    float prod = 0;
    for (int i = 0; i < count; i++) {
        prod += a[0] * b[0];
        a += step_a;
        b += step_b;
    }
    return prod / denom;
}

static float SFloatDot(int count, const float a[], int step_a,
                                       const float b[], int step_b) {
    float prod = 0;
    for (int i = 0; i < count; i++) {
        prod += a[0] * b[0];
        a += step_a;
        b += step_b;
    }
    return prod;
}

///////////////////////////////////////////////////////////////////////////////

float SkPoint3D::normalize(SkUnit3D* unit) const {
    float mag = SFloatSqrt(fX*fX + fY*fY + fZ*fZ);
    if (mag) {
        float scale = SFloatInvert(mag);
        unit->fX = fX * scale;
        unit->fY = fY * scale;
        unit->fZ = fZ * scale;
    } else {
        unit->fX = unit->fY = unit->fZ = 0;
    }
    return mag;
}

float SkUnit3D::Dot(const SkUnit3D& a, const SkUnit3D& b) {
    return a.fX * b.fX + a.fY * b.fY + a.fZ * b.fZ;
}

void SkUnit3D::Cross(const SkUnit3D& a, const SkUnit3D& b, SkUnit3D* cross) {
    SASSERT(cross);

    // use x,y,z, in case &a == cross or &b == cross

    float x = a.fY * b.fZ - a.fZ * b.fY;
    float y = a.fZ * b.fX - a.fX * b.fY;
    float z = a.fX * b.fY - a.fY * b.fX;

    cross->set(x, y, z);
}

///////////////////////////////////////////////////////////////////////////////

SkPatch3D::SkPatch3D() {
    this->reset();
}

void SkPatch3D::reset() {
    fOrigin.set(0, 0, 0);
    fU.set(SK_Scalar1, 0, 0);
    fV.set(0, -SK_Scalar1, 0);
}

void SkPatch3D::transform(const SkMatrix3D& m, SkPatch3D* dst) const {
    if (dst == NULL) {
        dst = (SkPatch3D*)this;
    }
    m.mapVector(fU, &dst->fU);
    m.mapVector(fV, &dst->fV);
    m.mapPoint(fOrigin, &dst->fOrigin);
}

float SkPatch3D::dotWith(float dx, float dy, float dz) const {
    float cx = SFloatMul(fU.fY, fV.fZ) - SFloatMul(fU.fZ, fV.fY);
    float cy = SFloatMul(fU.fZ, fV.fX) - SFloatMul(fU.fX, fV.fY);
    float cz = SFloatMul(fU.fX, fV.fY) - SFloatMul(fU.fY, fV.fX);

    return SFloatMul(cx, dx) + SFloatMul(cy, dy) + SFloatMul(cz, dz);
}

///////////////////////////////////////////////////////////////////////////////

void SkMatrix3D::reset() {
    memset(fMat, 0, sizeof(fMat));
    fMat[0][0] = fMat[1][1] = fMat[2][2] = SK_Scalar1;
}

void SkMatrix3D::setTranslate(float x, float y, float z) {
    memset(fMat, 0, sizeof(fMat));
    fMat[0][0] = x;
    fMat[1][1] = y;
    fMat[2][2] = z;
}

void SkMatrix3D::setRotateX(float degX) {
    float    s, c;

	s = SMatrix::SFloatSinCos(SkDegreesToRadians(degX), &c);
    this->setRow(0, SK_Scalar1, 0, 0);
    this->setRow(1, 0, c, -s);
    this->setRow(2, 0, s, c);
}

void SkMatrix3D::setRotateY(float degY) {
    float    s, c;

    s = SMatrix::SFloatSinCos(SkDegreesToRadians(degY), &c);
    this->setRow(0, c, 0, -s);
    this->setRow(1, 0, SK_Scalar1, 0);
    this->setRow(2, s, 0, c);
}

void SkMatrix3D::setRotateZ(float degZ) {
    float    s, c;

    s = SMatrix::SFloatSinCos(SkDegreesToRadians(degZ), &c);
    this->setRow(0, c, -s, 0);
    this->setRow(1, s, c, 0);
    this->setRow(2, 0, 0, SK_Scalar1);
}

void SkMatrix3D::preTranslate(float x, float y, float z) {
    float col[3] = { x, y, z};

    for (int i = 0; i < 3; i++) {
        fMat[i][3] += SFloatDot(3, &fMat[i][0], 1, col, 1);
    }
}

void SkMatrix3D::preRotateX(float degX) {
    SkMatrix3D m;
    m.setRotateX(degX);
    this->setConcat(*this, m);
}

void SkMatrix3D::preRotateY(float degY) {
    SkMatrix3D m;
    m.setRotateY(degY);
    this->setConcat(*this, m);
}

void SkMatrix3D::preRotateZ(float degZ) {
    SkMatrix3D m;
    m.setRotateZ(degZ);
    this->setConcat(*this, m);
}

void SkMatrix3D::setConcat(const SkMatrix3D& a, const SkMatrix3D& b) {
    SkMatrix3D  tmp;
    SkMatrix3D* c = this;

    if (this == &a || this == &b) {
        c = &tmp;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c->fMat[i][j] = SFloatDot(3, &a.fMat[i][0], 1, &b.fMat[0][j], 4);
        }
        c->fMat[i][3] = SFloatDot(3, &a.fMat[i][0], 1,
                                    &b.fMat[0][3], 4) + a.fMat[i][3];
    }

    if (c == &tmp) {
        *this = tmp;
    }
}

void SkMatrix3D::mapPoint(const SkPoint3D& src, SkPoint3D* dst) const {
    float x = SFloatDot(3, &fMat[0][0], 1, &src.fX, 1) + fMat[0][3];
    float y = SFloatDot(3, &fMat[1][0], 1, &src.fX, 1) + fMat[1][3];
    float z = SFloatDot(3, &fMat[2][0], 1, &src.fX, 1) + fMat[2][3];
    dst->set(x, y, z);
}

void SkMatrix3D::mapVector(const SkVector3D& src, SkVector3D* dst) const {
    float x = SFloatDot(3, &fMat[0][0], 1, &src.fX, 1);
    float y = SFloatDot(3, &fMat[1][0], 1, &src.fX, 1);
    float z = SFloatDot(3, &fMat[2][0], 1, &src.fX, 1);
    dst->set(x, y, z);
}

///////////////////////////////////////////////////////////////////////////////

SkCamera3D::SkCamera3D() {
    this->reset();
}

void SkCamera3D::reset() {
    fLocation.set(0, 0, -SkIntToScalar(576));   // 8 inches backward
    fAxis.set(0, 0, SK_Scalar1);                // forward
    fZenith.set(0, -SK_Scalar1, 0);             // up

    fObserver.set(0, 0, fLocation.fZ);

    fNeedToUpdate = true;
}

void SkCamera3D::update() {
    fNeedToUpdate = true;
}

void SkCamera3D::doUpdate() const {
    SkUnit3D    axis, zenith, cross;

    fAxis.normalize(&axis);

    {
        float dot = SkUnit3D::Dot(*SkTCast<const SkUnit3D*>(&fZenith), axis);

        zenith.fX = fZenith.fX - dot * axis.fX;
        zenith.fY = fZenith.fY - dot * axis.fY;
        zenith.fZ = fZenith.fZ - dot * axis.fZ;

        SkTCast<SkPoint3D*>(&zenith)->normalize(&zenith);
    }

    SkUnit3D::Cross(axis, zenith, &cross);

    {
        SMatrix* orien = &fOrientation;
        float x = fObserver.fX;
        float y = fObserver.fY;
        float z = fObserver.fZ;

        orien->set(kMScaleX, x * axis.fX - z * cross.fX);
        orien->set(kMSkewX,  x * axis.fY - z * cross.fY);
        orien->set(kMTransX, x * axis.fZ - z * cross.fZ);
        orien->set(kMSkewY,  y * axis.fX - z * zenith.fX);
        orien->set(kMScaleY, y * axis.fY - z * zenith.fY);
        orien->set(kMTransY, y * axis.fZ - z * zenith.fZ);
        orien->set(kMPersp0, axis.fX);
        orien->set(kMPersp1, axis.fY);
        orien->set(kMPersp2, axis.fZ);
    }
}

void SkCamera3D::patchToMatrix(const SkPatch3D& quilt, SMatrix* matrix) const {
    if (fNeedToUpdate) {
        this->doUpdate();
        fNeedToUpdate = false;
    }

    const float* mapPtr = fOrientation.fMat;
    const float* patchPtr;
    SkPoint3D       diff;
    float        dot;

    diff.fX = quilt.fOrigin.fX - fLocation.fX;
    diff.fY = quilt.fOrigin.fY - fLocation.fY;
    diff.fZ = quilt.fOrigin.fZ - fLocation.fZ;

    dot = SkUnit3D::Dot(*SkTCast<const SkUnit3D*>(&diff),
                        *SkTCast<const SkUnit3D*>(fOrientation.fMat + 6));

    patchPtr = (const float*)&quilt;
    matrix->set(kMScaleX, SFloatDotDiv(3, patchPtr, 1, mapPtr, 1, dot));
    matrix->set(kMSkewY,  SFloatDotDiv(3, patchPtr, 1, mapPtr+3, 1, dot));
    matrix->set(kMPersp0, SFloatDotDiv(3, patchPtr, 1, mapPtr+6, 1, dot));

    patchPtr += 3;
    matrix->set(kMSkewX,  SFloatDotDiv(3, patchPtr, 1, mapPtr, 1, dot));
    matrix->set(kMScaleY, SFloatDotDiv(3, patchPtr, 1, mapPtr+3, 1, dot));
    matrix->set(kMPersp1, SFloatDotDiv(3, patchPtr, 1, mapPtr+6, 1, dot));

    patchPtr = (const float*)(const void*)&diff;
    matrix->set(kMTransX, SFloatDotDiv(3, patchPtr, 1, mapPtr, 1, dot));
    matrix->set(kMTransY, SFloatDotDiv(3, patchPtr, 1, mapPtr+3, 1, dot));
    matrix->set(kMPersp2, SK_Scalar1);
}

///////////////////////////////////////////////////////////////////////////////

S3DView::S3DView() {
    fInitialRec.fMatrix.reset();
    fRec = &fInitialRec;
}

S3DView::~S3DView() {
    Rec* rec = fRec;
    while (rec != &fInitialRec) {
        Rec* next = rec->fNext;
        delete rec;
        rec = next;
    }
}

void S3DView::save() {
    Rec* rec = new Rec;
    rec->fNext = fRec;
    rec->fMatrix = fRec->fMatrix;
    fRec = rec;
}

void S3DView::restore() {
    SASSERT(fRec != &fInitialRec);
    Rec* next = fRec->fNext;
    delete fRec;
    fRec = next;
}

void S3DView::setCameraLocation(float x, float y, float z) {
    // the camera location is passed in inches, set in pt
    float lz = z * 72.0f;
    fCamera.fLocation.set(x * 72.0f, y * 72.0f, lz);
    fCamera.fObserver.set(0, 0, lz);
    fCamera.update();

}

float S3DView::getCameraLocationX() {
    return fCamera.fLocation.fX / 72.0f;
}

float S3DView::getCameraLocationY() {
    return fCamera.fLocation.fY / 72.0f;
}

float S3DView::getCameraLocationZ() {
    return fCamera.fLocation.fZ / 72.0f;
}

void S3DView::translate(float x, float y, float z) {
    fRec->fMatrix.preTranslate(x, y, z);
}

void S3DView::rotateX(float deg) {
    fRec->fMatrix.preRotateX(deg);
}

void S3DView::rotateY(float deg) {
    fRec->fMatrix.preRotateY(deg);
}

void S3DView::rotateZ(float deg) {
    fRec->fMatrix.preRotateZ(deg);
}

float S3DView::dotWithNormal(float x, float y, float z) const {
    SkPatch3D   patch;
    patch.transform(fRec->fMatrix);
    return patch.dotWith(x, y, z);
}

void S3DView::getMatrix(SMatrix* matrix) const {
    if (matrix != NULL) {
        SkPatch3D   patch;
        patch.transform(fRec->fMatrix);
        fCamera.patchToMatrix(patch, matrix);
    }
}

////////////////////////////////////////////////////////////////////
SCamera::SCamera():m_priv(new(S3DView))
{

}

SCamera::~SCamera()
{
	delete m_priv;
}

void SCamera::save()
{
	m_priv->save();
}

void SCamera::restore()
{
	m_priv->restore();
}

void SCamera::translate(float x, float y, float z)
{
	m_priv->translate(x, y, z);
}

void SCamera::rotateX(float deg)
{
	m_priv->rotateX(deg);
}

void SCamera::rotateY(float deg)
{
	m_priv->rotateY(deg);
}

void SCamera::rotateZ(float deg)
{
	m_priv->rotateZ(deg);
}

void SCamera::setCameraLocation(float x, float y, float z)
{
	m_priv->setCameraLocation(x, y, z);
}

float SCamera::getCameraLocationX()
{
	return m_priv->getCameraLocationX();;
}

float SCamera::getCameraLocationY()
{
	return m_priv->getCameraLocationY();
}

float SCamera::getCameraLocationZ()
{
	return m_priv->getCameraLocationZ();
}

void SCamera::getMatrix(SMatrix * out) const
{
	return m_priv->getMatrix(out);
}

}//end of namespace SOUI
