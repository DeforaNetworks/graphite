/*  GRAPHITE2 LICENSING

    Copyright 2010, SIL International
    All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, Inc., 59 Temple Place, 
    Suite 330, Boston, MA 02111-1307, USA or visit their web page on the 
    internet at http://www.fsf.org/licenses/lgpl.html.

Alternatively, the contents of this file may be used under the terms of the
Mozilla Public License (http://mozilla.org/MPL) or the GNU General Public
License, as published by the Free Software Foundation, either version 2
of the License or (at your option) any later version.
*/
#include <iterator>
#include "GlyphFace.h"
#include "XmlTraceLog.h"
#include "GlyphFaceCache.h"
#include "TtfUtil.h"


using namespace graphite2;

namespace
{
	struct glyph_attr { uint16 id, value; };

	class glat_iterator : public std::iterator<std::input_iterator_tag, glyph_attr>
	{
	public:
		glat_iterator() : _p(0), _n(0) {}
		glat_iterator(const void * glat) : _p(reinterpret_cast<const byte *>(glat)), _n(0) {
			operator++();
		}

		glat_iterator & operator ++ () 		{ ++_v.id; if (_n==0) { _v.id = *_p++; _n = *_p++; } --_n; _v.value = read16(_p); return *this; }
		glat_iterator   operator ++ (int) 	{ glat_iterator tmp(*this); operator++(); return tmp; }

		bool operator == (const glat_iterator & rhs) { return _p == rhs._p; }
		bool operator != (const glat_iterator & rhs) { return !operator==(rhs); }

		value_type 			operator * () const { return _v; }
		const value_type *	operator ->() const { return &_v; }

	protected:
		const byte * _p;
		value_type  _v;
		unsigned int _n;
	};

	class glat2_iterator : public glat_iterator
	{
	public:
		glat2_iterator(const void * glat) { _p = reinterpret_cast<const byte *>(glat); _n = 0; operator++(); }

		glat_iterator & operator ++ () 		{ ++_v.id; if (!_n) { _v.id = read16(_p); _n = read16(_p); } --_n; _v.value = read16(_p); return *this; }
		glat_iterator   operator ++ (int) 	{ glat_iterator tmp(*this); operator++(); return tmp; }
	};
}

GlyphFace::GlyphFace(const GlyphFaceCacheHeader& hdr, unsigned short glyphid)
{
    if (glyphid < hdr.m_nGlyphsWithGraphics)
    {
        int nLsb, xMin, yMin, xMax, yMax;
        unsigned int nAdvWid;
        size_t locidx = TtfUtil::LocaLookup(glyphid, hdr.m_pLoca, hdr.m_lLoca, hdr.m_pHead);
        void *pGlyph = TtfUtil::GlyfLookup(hdr.m_pGlyf, locidx, hdr.m_lGlyf);
        if (TtfUtil::HorMetrics(glyphid, hdr.m_pHmtx, hdr.m_lHmtx, hdr.m_pHHea, nLsb, nAdvWid))
            m_advance = Position(static_cast<float>(nAdvWid), 0);
        else
            m_advance = Position();
        if (pGlyph && TtfUtil::GlyfBox(pGlyph, xMin, yMin, xMax, yMax))
            m_bbox = Rect(Position(static_cast<float>(xMin), static_cast<float>(yMin)),
//                    Position(static_cast<float>(xMax - xMin), static_cast<float>(yMax - yMin)));
                Position(static_cast<float>(xMax), static_cast<float>(yMax)));
        else
            m_bbox = Rect();
    }
    else
    {
        m_advance = Position();
        m_bbox = Rect();
    }
#ifndef DISABLE_TRACING
    if (XmlTraceLog::get().active())
    {
        XmlTraceLog::get().openElement(ElementGlyphFace);
        XmlTraceLog::get().addAttribute(AttrGlyphId, glyphid);
        XmlTraceLog::get().addAttribute(AttrAdvanceX, m_advance.x);
        XmlTraceLog::get().addAttribute(AttrAdvanceY, m_advance.y);
    }
#endif
    if (glyphid < hdr.m_nGlyphsWithAttributes)
    {
        size_t glocs, gloce;
        if (hdr.m_locFlagsUse32Bit)
        {
            glocs = swap32(((uint32 *)hdr.m_pGloc)[2+glyphid]);
            gloce = swap32(((uint32 *)hdr.m_pGloc)[3+glyphid]);
        }
        else
        {
            glocs = swap16(((uint16 *)hdr.m_pGloc)[4+glyphid]);
            gloce = swap16(((uint16 *)hdr.m_pGloc)[5+glyphid]);
        }
        if (glocs < hdr.m_lGlat && gloce <= hdr.m_lGlat)
        {
//            m_attrs = grzeroalloc<uint16>(hdr.m_numAttrs);
//            readAttrs(hdr.m_pGlat + glocs, hdr.m_pGlat + gloce, m_attrs, hdr.m_numAttrs, hdr.m_fGlat);
        	if (hdr.m_fGlat < 0x00020000)
        		new (&m_attrs) sparse(glat_iterator(hdr.m_pGlat + glocs), glat_iterator(hdr.m_pGlat + gloce));
        	else
        		new (&m_attrs) sparse(glat2_iterator(hdr.m_pGlat + glocs), glat2_iterator(hdr.m_pGlat + gloce));

        }
    }
#ifndef DISABLE_TRACING
    XmlTraceLog::get().closeElement(ElementGlyphFace);
#endif
}


//void GlyphFace::readAttrs(const byte *glat, const byte * const glat_end, uint16 * attrs, size_t num, uint32 format)
//{
//	glat_iterator::value_type v;
//
//	if (format < 0x00020000)
//	{
//		for (glat_iterator i = glat, end = glat_end; i != end; ++i)
//		{
//			v = *i;
//			if (v.id >= num) break;
//			attrs[v.id] = v.value;
//		}
//	}
//	else
//	{
//		for (glat2_iterator i = glat, end = glat_end; i != end; ++i)
//		{
//			v = *i;
//			if (v.id >= num) break;
//			attrs[v.id] = v.value;
//		}
//	}
//
//	if (v.id >= num) {
//#ifndef DISABLE_TRACING
//		XmlTraceLog::get().warning("Invalid glat entry: attr id %d max id %d", v.id, num);
//#endif
//	}
//
//	const byte * 		glat 		= static_cast<const byte *>(pGlat) + start,
//			   * const 	glat_end 	= glat + end - start;


//	while (glat < glat_end)
//	{
//		uint16 * attr, * attr_end;
//
//		if (format < 0x00020000)
//		{
//			attr = attrs + *glat++; attr_end = attr + *glat++;
//		}
//		else
//		{
//			attr = attrs + read16(glat); attr_end = attr + read16(glat);
//		}
//
//		if (attr_end > attrs + num)
//		{
//#ifndef DISABLE_TRACING
//			XmlTraceLog::get().warning("Invalid glat entry: attr id %d count %d", *attr, attr_end - attr);
//#endif
//			return;
//		}
//		while (attr != attr_end)
//		{
//			*attr++ = read16(glat);
//			logAttr(attrs, attr-1);
//		}
//	}
//}


inline
void GlyphFace::logAttr(GR_MAYBE_UNUSED const uint16 attrs[], GR_MAYBE_UNUSED const uint16 * attr)
{
#ifndef DISABLE_TRACING
	if (XmlTraceLog::get().active())
	{
		XmlTraceLog::get().openElement(ElementAttr);
		XmlTraceLog::get().addAttribute(AttrAttrId, attr - attrs);
		XmlTraceLog::get().addAttribute(AttrAttrVal, *attr);
		XmlTraceLog::get().closeElement(ElementAttr);
	}
#endif
}


uint16 GlyphFace::getMetric(uint8 metric) const
{
    switch (metrics(metric))
    {
        case kgmetLsb : return static_cast<uint16>(m_bbox.bl.x);
        case kgmetRsb : return static_cast<uint16>(m_advance.x - m_bbox.tr.x);
        case kgmetBbTop : return static_cast<uint16>(m_bbox.tr.y);
        case kgmetBbBottom : return static_cast<uint16>(m_bbox.bl.y);
        case kgmetBbLeft : return static_cast<uint16>(m_bbox.bl.x);
        case kgmetBbRight : return static_cast<uint16>(m_bbox.tr.x);
        case kgmetBbHeight: return static_cast<uint16>(m_bbox.tr.y - m_bbox.bl.y);
        case kgmetBbWidth : return static_cast<uint16>(m_bbox.tr.x - m_bbox.bl.x);
        case kgmetAdvWidth : return static_cast<uint16>(m_advance.x);
        case kgmetAdvHeight : return static_cast<uint16>(m_advance.y);
        default : return 0;
    }
}
