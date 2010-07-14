#include "GlyphFaceCache.h"
#include "graphiteng/IFace.h"
#include "GrFace.h"     //for the tags

using namespace org::sil::graphite::v2;



/*virtual*/ bool GlyphFaceCacheHeader::initialize(const IFace* iFace/*not NULL*/)    //return result indicates success. Do not use if failed.
{
    if ((m_pLoca = iFace->getTable(tagLoca, &m_lLoca)) == NULL) return false;
    size_t lHead;
    if ((m_pHead = iFace->getTable(tagHead, &lHead)) == NULL) return false;
    size_t lGlyf;
    if ((m_pGlyf = iFace->getTable(tagGlyf, &lGlyf)) == NULL) return false;
    if ((m_pHmtx = iFace->getTable(tagHmtx, &m_lHmtx)) == NULL) return false;
    size_t lHHea;
    if ((m_pHHea = iFace->getTable(tagHhea, &lHHea)) == NULL) return false;
    size_t lGlat;
    if ((m_pGlat = iFace->getTable(tagGlat, &lGlat)) == NULL) return false;

    size_t lMaxp;
    const void* pMaxp = iFace->getTable(tagMaxp, &lMaxp);
    if (pMaxp==NULL) return false;
    m_nGlyphsWithGraphics = (unsigned short)TtfUtil::GlyphCount(pMaxp);
    
    size_t lGloc;
    if ((m_pGloc = iFace->getTable(tagGloc, &lGloc)) == NULL) return false;
    if (lGloc < 6) return false;
    int version = swap32(*((uint32 *)m_pGloc));
    if (version != 0x00010000) return false;

    m_numAttrs = swap16(((uint16 *)m_pGloc)[3]);

    unsigned short locFlags = swap16(((uint16 *)m_pGloc)[2]);
    if (locFlags&1)
    {
        m_locFlagsUse32Bit = true;
        m_nGlyphsWithAttributes = (unsigned short)((lGloc - 10) / 4);
    }
    else
    {
        m_locFlagsUse32Bit = false;
        m_nGlyphsWithAttributes = (unsigned short)((lGloc - 8) / 2);
    }
    
    if (m_nGlyphsWithAttributes>m_nGlyphsWithGraphics) 
        m_nGlyphs = m_nGlyphsWithAttributes;
    else
        m_nGlyphs = m_nGlyphsWithGraphics;

    return true;
}


GlyphFaceCachePreloaded::GlyphFaceCachePreloaded()
:   m_glyphs(NULL)
{
}

/*virtual*/ GlyphFaceCachePreloaded::~GlyphFaceCachePreloaded()
{
//    delete[] m_glyphs;        //can't do this since not allocated by new[] and so does not know array size.
    if (!m_glyphs)
        return ;
    
    unsigned int nGlyphs = numGlyphs();
    for (unsigned int i=0 ; i<nGlyphs; ++i)
        m_glyphs[i].~GlyphFace();
}


/*virtual*/ bool GlyphFaceCachePreloaded::initialize(const IFace* iFace/*not NULL*/)    //return result indicates success. Do not use if failed.
{
    if (!GlyphFaceCache::initialize(iFace))
        return false;

    unsigned int nGlyphs = numGlyphs();
    m_glyphs = (GlyphFace*)malloc(sizeof(GlyphFace)*nGlyphs);
    if (!m_glyphs)
        return false;
    
#ifndef DISABLE_TRACING
    if (XmlTraceLog::get().active())
    {
        XmlTraceLog::get().openElement(ElementGlyphs);
        XmlTraceLog::get().addAttribute(AttrNum, nGlyphs);
    }
#endif
    for (unsigned int i = 0; i < nGlyphs; i++)
    {
        new(m_glyphs + i) GlyphFace(*this, i);
    }
#ifndef DISABLE_TRACING
    XmlTraceLog::get().closeElement(ElementGlyphs);
#endif
    return true;
}
    


/*virtual*/ const GlyphFace *GlyphFaceCachePreloaded::glyph(unsigned short glyphid) const      //result may be changed by subsequent call with a different glyphid
{ 
    return m_glyphs + glyphid; 
}
