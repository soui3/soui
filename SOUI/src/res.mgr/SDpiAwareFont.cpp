#include "souistd.h"
#include "res.mgr/SDpiAwareFont.h"

namespace SOUI
{
	static const SStringW KNoneFont = L"none";
	SDpiAwareFont::SDpiAwareFont(void):m_strDesc(KNoneFont)
	{
	}

	SDpiAwareFont::~SDpiAwareFont(void)
	{
	}

	IFont * SDpiAwareFont::GetFontPtr()
	{
		return m_object;
	}

	void SDpiAwareFont::SetFontDesc(const SStringW & strDesc, int nScale)
	{
		m_strDesc = strDesc;
		m_object = OnGetFont(m_strDesc, nScale);
	}

	void SDpiAwareFont::UpdateFont(int nScale)
	{
		if (m_strDesc == KNoneFont) return;
		m_object = OnGetFont(m_strDesc, nScale);
	}

	IFont * SDpiAwareFont::OnGetFont(const SStringW & strDesc,int nScale)
	{
		return SFontPool::getSingleton().GetFont(strDesc,nScale);
	}
}
