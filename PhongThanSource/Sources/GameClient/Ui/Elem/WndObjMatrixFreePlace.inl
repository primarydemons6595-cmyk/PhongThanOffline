bool KWndObjectMatrix::FindFreePlace(int width, int height, int& left, int& top) const
{
    if (width <= 0 || height <= 0 || width > m_nNumUnitHori || height > m_nNUmUnitVert)
        return false;
    for (int y = 0; y <= m_nNUmUnitVert - height; ++y)
        for (int x = 0; x <= m_nNumUnitHori - width; ++x)
        {
            bool free = true;
            for (int i = 0; i < m_nNumObjects; ++i)
            {
                const KUiDraggedObject& obj = m_pObjects[i];
                if (obj.DataW <= 0 || obj.DataH <= 0 || obj.DataX < 0 || obj.DataY < 0 ||
                    obj.DataW > m_nNumUnitHori || obj.DataH > m_nNUmUnitVert ||
                    obj.DataX > m_nNumUnitHori - obj.DataW || obj.DataY > m_nNUmUnitVert - obj.DataH)
                    return false;
                if (x < obj.DataX + obj.DataW && x + width > obj.DataX &&
                    y < obj.DataY + obj.DataH && y + height > obj.DataY)
                { free = false; break; }
            }
            if (free) { left = x; top = y; return true; }
        }
    return false;
}
