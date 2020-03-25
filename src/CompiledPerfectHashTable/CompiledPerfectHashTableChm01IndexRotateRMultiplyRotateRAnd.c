
DECLARE_INDEX_ROUTINE()
{
    CPHINDEX Index;
    CPHDKEY Vertex1;
    CPHDKEY Vertex2;
    CPHDKEY MaskedLow;
    CPHDKEY MaskedHigh;
    CPHDKEY DownsizedKey;

    DownsizedKey = DOWNSIZE_KEY(Key);

    Vertex1 = DownsizedKey;
    Vertex1 = _rotr(Vertex1, SEED3_BYTE1);
    Vertex1 *= SEED1;
    Vertex1 = _rotr(Vertex1, SEED3_BYTE3);

    Vertex2 = DownsizedKey;
    Vertex2 = _rotr(Vertex2, SEED3_BYTE2);
    Vertex2 *= SEED2;
    Vertex2 = _rotr(Vertex2, SEED3_BYTE4);

    MaskedLow = Vertex1 & HASH_MASK;
    MaskedHigh = Vertex2 & HASH_MASK;

    Vertex1 = TABLE_DATA[MaskedLow];
    Vertex2 = TABLE_DATA[MaskedHigh];

    Index = (CPHINDEX)((Vertex1 + Vertex2) & INDEX_MASK);

    return Index;
}

