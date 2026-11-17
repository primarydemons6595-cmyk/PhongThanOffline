// Shared by simulation and paint; consume a rebuild request before traversal.
// Clear first so a new request arriving during Preprocess remains pending.
if (m_bPreprocessEvent)
{
    m_bPreprocessEvent = false;
    Preprocess();
}
