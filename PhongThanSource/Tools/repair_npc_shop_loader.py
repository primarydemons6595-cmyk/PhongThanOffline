"""Keep original goods row IDs stable, including unsupported entries."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=root/'Sources/Core/Src/KBuySell.cpp'
data=p.read_bytes().replace(b'\r\n',b'\n')
def replace(old,new):
    global data
    a=old.encode('ascii');b=new.encode('ascii')
    if b in data:return
    if data.count(a)!=1:raise RuntimeError(old)
    data=data.replace(a,b)
replace('#include "KEngine.h"','#include <vector>\n#include "KEngine.h"')
replace('\tItemGenerate itemKey;\n\tfor (int k',
        '\t// BuySell references original one-based goods rows, not generated indexes.\n\tstd::vector<int> normalGoodsIndex(nGoodsHeight, -1);\n\tItemGenerate itemKey;\n\tfor (int k')
replace('\t\tm_Item[m_ItemNum].GetBackLocal()->Release();\n\t\t++m_ItemNum;\n\t}',
        '\t\tm_Item[m_ItemNum].GetBackLocal()->Release();\n\t\tnormalGoodsIndex[k] = m_ItemNum;\n\t\t++m_ItemNum;\n\t}')
start=data.index(b'\tFILE* normalShopLog =') if b'\tFILE* normalShopLog =' in data else data.index(b'\tfor (int nNormalRow = 0;')
end=data.index(b'\n\tif (pIBShopItemIndex)',start)
data=data[:start]+b'''\tFILE* normalShopLog = fopen("normal_shop_loader_diag.csv", "wt");
\tif(normalShopLog)fprintf(normalShopLog,"shop,slot,goods_row,item_index,genre,detail\\n");
\tfor (int nNormalRow = 0; nNormalRow < nNormalShopHeight; ++nNormalRow)
\t{
\t\tfor (int column = 0; column < BuySellFile.GetWidth(); ++column)
\t\t{
\t\t\tint rowId = -1;
\t\t\t// VNG BuySell has goods in column one; it has no shop-ID column.
\t\t\tBuySellFile.GetInteger(nNormalRow + 2, column + 1, -1, &rowId);
\t\t\tif (rowId > 0 && rowId <= nGoodsHeight)
\t\t\t{
\t\t\t\tm_SellItem[nNormalRow][column] = normalGoodsIndex[rowId - 1];
\t\t\t\tconst int index=normalGoodsIndex[rowId - 1];
\t\t\t\tif(normalShopLog)fprintf(normalShopLog,"%d,%d,%d,%d,%d,%d\\n",nNormalRow+1,column,rowId,index,
\t\t\t\t\tindex>=0?m_Item[index].GetGenre():-1,index>=0?m_Item[index].GetDetailType():-1);
\t\t\t}
\t\t}
\t}
\tif(normalShopLog)fclose(normalShopLog);
'''+data[end:]
p.write_bytes(data)
print('Shop original row references and first-column loading repaired.')
