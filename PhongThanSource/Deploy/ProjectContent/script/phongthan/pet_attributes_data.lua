-- Original attribute-pet data; Lua 4 isolated namespace.
PTAP_PetRoleTask = 2109

PTAP_Value_AblePet = 2071

PTAP_Task_AblePet = 2072
PTAP_Break_AblePet = 2073

PTAP_NeZha_Pet = 2078

PTAP_Task_NeZhaPet = 2079
PTAP_Break_NeZhaPet = 2080

PTAP_LeiZhenZi_Pet = 2091

PTAP_Task_LeiZhenZiPet = 2092
PTAP_Break_LeiZhenZiPet = 2093

PTAP_ShiJi_Pet = 2106

PTAP_Task_ShiJiPet = 2107
PTAP_Break_ShiJiPet = 2108

PTAP_TaiYi_Pet = 2114

PTAP_Task_TaiYiPet = 2115
PTAP_Break_TaiYiPet = 2116

PTAP_DaJi_Pet = 2117

PTAP_Task_DaJiPet = 2118
PTAP_Break_DaJiPet = 2119

PTAP_ShenGongBao_Pet = 2120

PTAP_Task_ShenGongBaoPet = 2121
PTAP_Break_ShenGongBaoPet = 2122

PTAP_HuangFeiHu_Pet = 2123

PTAP_Task_HuangFeiHuPet = 2124
PTAP_Break_HuangFeiHuPet = 2125

PTAP_TaskTable = {
  [1] = { num = 10000, id = { 6, 1, 1339, 1 }, monster = 90, spe = 0, buffid = 1784, petid = 46, petexp = 0.3, petmoney = 3000 * 0.15, useTask = { 2188, 1 }, },
  [2] = { num = 20000, id = { 6, 1, 1340, 1 }, monster = 90, spe = 0, buffid = 1785, petid = 46, petexp = 0.4, petmoney = 3000 * 0.2, useTask = { 2188, 2 }, },
  [3] = { num = 30000, id = { 6, 1, 1341, 1 }, monster = 90, spe = 1, buffid = 1786, petid = 46, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 3 }, },
  [4] = { num = 40000, id = { 6, 1, 1342, 1 }, monster = 100, spe = 0, buffid = 1787, petid = 47, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 4 }, },
  [5] = { num = 50000, id = { 6, 1, 1343, 1 }, monster = 100, spe = 0, buffid = 1788, petid = 47, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 5 }, },
  [6] = { num = 60000, id = { 6, 1, 1344, 1 }, monster = 100, spe = 1, buffid = 1789, petid = 47, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 6 }, },
  [7] = { num = 70000, id = { 6, 1, 1345, 1 }, monster = 110, spe = 0, buffid = 1790, petid = 48, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 7 }, },
  [8] = { num = 80000, id = { 6, 1, 1346, 1 }, monster = 110, spe = 0, buffid = 1791, petid = 48, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 8 }, },
  [9] = { num = 90000, id = { 6, 1, 1347, 1 }, monster = 110, spe = 1, buffid = 1792, petid = 48, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 9 }, },
  [10] = { num = 90000, id = { 6, 1, 1348, 1 }, monster = 110, spe = 0, buffid = 1793, petid = 49, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 10 }, },
  [11] = { num = 90000, id = { 6, 1, 1541, 1 }, monster = 110, spe = 0, buffid = 1936, petid = 83, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2191, 1 }, },
}

PTAP_TaskTable_NeZha = {
  [1] = { num = 10000, id = { 6, 1, 1359, 1 }, monster = 90, spe = 0, buffid = 1808, petid = 50, petexp = 0.3, petmoney = 3000 * 0.15, useTask = { 2188, 11 }, },
  [2] = { num = 20000, id = { 6, 1, 1360, 1 }, monster = 90, spe = 0, buffid = 1809, petid = 50, petexp = 0.4, petmoney = 3000 * 0.2, useTask = { 2188, 12 }, },
  [3] = { num = 30000, id = { 6, 1, 1361, 1 }, monster = 90, spe = 1, buffid = 1810, petid = 50, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 13 }, },
  [4] = { num = 40000, id = { 6, 1, 1362, 1 }, monster = 100, spe = 0, buffid = 1811, petid = 51, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 14 }, },
  [5] = { num = 50000, id = { 6, 1, 1363, 1 }, monster = 100, spe = 0, buffid = 1812, petid = 51, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 15 }, },
  [6] = { num = 60000, id = { 6, 1, 1364, 1 }, monster = 100, spe = 1, buffid = 1813, petid = 51, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 16 }, },
  [7] = { num = 70000, id = { 6, 1, 1365, 1 }, monster = 110, spe = 0, buffid = 1814, petid = 52, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 17 }, },
  [8] = { num = 80000, id = { 6, 1, 1366, 1 }, monster = 110, spe = 0, buffid = 1815, petid = 52, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 18 }, },
  [9] = { num = 90000, id = { 6, 1, 1367, 1 }, monster = 110, spe = 1, buffid = 1816, petid = 52, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 19 }, },
  [10] = { num = 90000, id = { 6, 1, 1368, 1 }, monster = 110, spe = 0, buffid = 1817, petid = 53, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 20 }, },
  [11] = { num = 90000, id = { 6, 1, 1542, 1 }, monster = 110, spe = 0, buffid = 1937, petid = 84, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2191, 2 }, },
}

PTAP_TaskTable_LeiZhenZi = {
  [1] = { num = 10000, id = { 6, 1, 1426, 1 }, monster = 90, spe = 0, buffid = 1833, petid = 58, petexp = 0.3, petmoney = 3000 * 0.15, useTask = { 2188, 21 }, },
  [2] = { num = 20000, id = { 6, 1, 1427, 1 }, monster = 90, spe = 0, buffid = 1834, petid = 58, petexp = 0.4, petmoney = 3000 * 0.2, useTask = { 2188, 22 }, },
  [3] = { num = 30000, id = { 6, 1, 1428, 1 }, monster = 90, spe = 1, buffid = 1835, petid = 58, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 23 }, },
  [4] = { num = 40000, id = { 6, 1, 1429, 1 }, monster = 100, spe = 0, buffid = 1836, petid = 59, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 24 }, },
  [5] = { num = 50000, id = { 6, 1, 1430, 1 }, monster = 100, spe = 0, buffid = 1837, petid = 59, petexp = 0.5, petmoney = 3000 * 0.25, useTask = { 2188, 25 }, },
  [6] = { num = 60000, id = { 6, 1, 1431, 1 }, monster = 100, spe = 1, buffid = 1838, petid = 59, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 26 }, },
  [7] = { num = 70000, id = { 6, 1, 1432, 1 }, monster = 110, spe = 0, buffid = 1839, petid = 60, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 27 }, },
  [8] = { num = 80000, id = { 6, 1, 1433, 1 }, monster = 110, spe = 0, buffid = 1840, petid = 60, petexp = 0.8, petmoney = 3000 * 0.4, useTask = { 2188, 28 }, },
  [9] = { num = 90000, id = { 6, 1, 1434, 1 }, monster = 110, spe = 1, buffid = 1841, petid = 60, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 29 }, },
  [10] = { num = 90000, id = { 6, 1, 1435, 1 }, monster = 110, spe = 0, buffid = 1842, petid = 61, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2188, 30 }, },
  [11] = { num = 90000, id = { 6, 1, 1543, 1 }, monster = 110, spe = 0, buffid = 1938, petid = 85, petexp = 1, petmoney = 3000 * 0.5, useTask = { 2191, 3 }, },
}

PTAP_TaskTable_ShiJi = {
  [1] = { num = 10000, id = { 6, 1, 1448, 1 }, monster = 90, spe = 0, buffid = 1847, petid = 62, petexp = 0.3, petmoney = 0, useTask = { 2188, 31 }, },
  [2] = { num = 20000, id = { 6, 1, 1449, 1 }, monster = 90, spe = 0, buffid = 1848, petid = 62, petexp = 0.4, petmoney = 0, useTask = { 2189, 1 }, },
  [3] = { num = 30000, id = { 6, 1, 1450, 1 }, monster = 90, spe = 1, buffid = 1849, petid = 62, petexp = 0.5, petmoney = 0, useTask = { 2189, 2 }, },
  [4] = { num = 40000, id = { 6, 1, 1451, 1 }, monster = 100, spe = 0, buffid = 1850, petid = 63, petexp = 0.5, petmoney = 0, useTask = { 2189, 3 }, },
  [5] = { num = 50000, id = { 6, 1, 1452, 1 }, monster = 100, spe = 0, buffid = 1851, petid = 63, petexp = 0.5, petmoney = 0, useTask = { 2189, 4 }, },
  [6] = { num = 60000, id = { 6, 1, 1453, 1 }, monster = 100, spe = 1, buffid = 1852, petid = 63, petexp = 0.8, petmoney = 0, useTask = { 2189, 5 }, },
  [7] = { num = 70000, id = { 6, 1, 1454, 1 }, monster = 110, spe = 0, buffid = 1853, petid = 64, petexp = 0.8, petmoney = 0, useTask = { 2189, 6 }, },
  [8] = { num = 80000, id = { 6, 1, 1455, 1 }, monster = 110, spe = 0, buffid = 1854, petid = 64, petexp = 0.8, petmoney = 0, useTask = { 2189, 7 }, },
  [9] = { num = 90000, id = { 6, 1, 1456, 1 }, monster = 110, spe = 1, buffid = 1855, petid = 64, petexp = 1, petmoney = 0, useTask = { 2189, 8 }, },
  [10] = { num = 90000, id = { 6, 1, 1457, 1 }, monster = 110, spe = 0, buffid = 1856, petid = 65, petexp = 1, petmoney = 0, useTask = { 2189, 9 }, },
  [11] = { num = 90000, id = { 6, 1, 1544, 1 }, monster = 110, spe = 0, buffid = 1939, petid = 86, petexp = 1, petmoney = 0, useTask = { 2191, 4 }, },
}

PTAP_TaskTable_TaiYi = {
  [1] = { num = 10000, id = { 6, 1, 1465, 1 }, monster = 90, spe = 0, buffid = 1861, petid = 66, petexp = 0.3, petmoney = 0, useTask = { 2189, 10 }, },
  [2] = { num = 20000, id = { 6, 1, 1466, 1 }, monster = 90, spe = 0, buffid = 1862, petid = 66, petexp = 0.4, petmoney = 0, useTask = { 2189, 11 }, },
  [3] = { num = 30000, id = { 6, 1, 1467, 1 }, monster = 90, spe = 1, buffid = 1863, petid = 66, petexp = 0.5, petmoney = 0, useTask = { 2189, 12 }, },
  [4] = { num = 40000, id = { 6, 1, 1468, 1 }, monster = 100, spe = 0, buffid = 1864, petid = 67, petexp = 0.5, petmoney = 0, useTask = { 2189, 13 }, },
  [5] = { num = 50000, id = { 6, 1, 1469, 1 }, monster = 100, spe = 0, buffid = 1865, petid = 67, petexp = 0.5, petmoney = 0, useTask = { 2189, 14 }, },
  [6] = { num = 60000, id = { 6, 1, 1470, 1 }, monster = 100, spe = 1, buffid = 1866, petid = 67, petexp = 0.8, petmoney = 0, useTask = { 2189, 15 }, },
  [7] = { num = 70000, id = { 6, 1, 1471, 1 }, monster = 110, spe = 0, buffid = 1867, petid = 68, petexp = 0.8, petmoney = 0, useTask = { 2189, 16 }, },
  [8] = { num = 80000, id = { 6, 1, 1472, 1 }, monster = 110, spe = 0, buffid = 1868, petid = 68, petexp = 0.8, petmoney = 0, useTask = { 2189, 17 }, },
  [9] = { num = 90000, id = { 6, 1, 1473, 1 }, monster = 110, spe = 1, buffid = 1869, petid = 68, petexp = 1, petmoney = 0, useTask = { 2189, 18 }, },
  [10] = { num = 90000, id = { 6, 1, 1474, 1 }, monster = 110, spe = 0, buffid = 1870, petid = 69, petexp = 1, petmoney = 0, useTask = { 2189, 19 }, },
  [11] = { num = 90000, id = { 6, 1, 1475, 1 }, monster = 110, spe = 0, buffid = 1940, petid = 70, petexp = 1, petmoney = 0, useTask = { 2191, 5 }, },
}

PTAP_TaskTable_DaJi = {
  [1] = { num = 10000, id = { 6, 1, 1478, 1 }, monster = 90, spe = 0, buffid = 1879, petid = 71, petexp = 0.3, petmoney = 0, useTask = { 2189, 20 }, },
  [2] = { num = 20000, id = { 6, 1, 1479, 1 }, monster = 90, spe = 0, buffid = 1880, petid = 71, petexp = 0.4, petmoney = 0, useTask = { 2189, 21 }, },
  [3] = { num = 30000, id = { 6, 1, 1480, 1 }, monster = 90, spe = 1, buffid = 1881, petid = 71, petexp = 0.5, petmoney = 0, useTask = { 2189, 22 }, },
  [4] = { num = 40000, id = { 6, 1, 1481, 1 }, monster = 100, spe = 0, buffid = 1882, petid = 72, petexp = 0.5, petmoney = 0, useTask = { 2189, 23 }, },
  [5] = { num = 50000, id = { 6, 1, 1482, 1 }, monster = 100, spe = 0, buffid = 1883, petid = 72, petexp = 0.5, petmoney = 0, useTask = { 2189, 24 }, },
  [6] = { num = 60000, id = { 6, 1, 1483, 1 }, monster = 100, spe = 1, buffid = 1884, petid = 72, petexp = 0.8, petmoney = 0, useTask = { 2189, 25 }, },
  [7] = { num = 70000, id = { 6, 1, 1484, 1 }, monster = 110, spe = 0, buffid = 1885, petid = 73, petexp = 0.8, petmoney = 0, useTask = { 2189, 26 }, },
  [8] = { num = 80000, id = { 6, 1, 1485, 1 }, monster = 110, spe = 0, buffid = 1886, petid = 73, petexp = 0.8, petmoney = 0, useTask = { 2189, 27 }, },
  [9] = { num = 90000, id = { 6, 1, 1486, 1 }, monster = 110, spe = 1, buffid = 1887, petid = 73, petexp = 1, petmoney = 0, useTask = { 2189, 28 }, },
  [10] = { num = 90000, id = { 6, 1, 1487, 1 }, monster = 110, spe = 0, buffid = 1888, petid = 74, petexp = 1, petmoney = 0, useTask = { 2189, 29 }, },
  [11] = { num = 90000, id = { 6, 1, 1659, 1 }, monster = 110, spe = 0, buffid = 2007, petid = 109, petexp = 1, petmoney = 0, useTask = { 2191, 6 }, },
}

PTAP_TaskTable_ShenGongBao = {
  [1] = { num = 10000, id = { 6, 1, 1488, 1 }, monster = 90, spe = 0, buffid = 1889, petid = 75, petexp = 0.3, petmoney = 0, useTask = { 2189, 30 }, },
  [2] = { num = 20000, id = { 6, 1, 1489, 1 }, monster = 90, spe = 0, buffid = 1890, petid = 75, petexp = 0.4, petmoney = 0, useTask = { 2189, 31 }, },
  [3] = { num = 30000, id = { 6, 1, 1490, 1 }, monster = 90, spe = 1, buffid = 1891, petid = 75, petexp = 0.5, petmoney = 0, useTask = { 2190, 1 }, },
  [4] = { num = 40000, id = { 6, 1, 1491, 1 }, monster = 100, spe = 0, buffid = 1892, petid = 76, petexp = 0.5, petmoney = 0, useTask = { 2190, 2 }, },
  [5] = { num = 50000, id = { 6, 1, 1492, 1 }, monster = 100, spe = 0, buffid = 1893, petid = 76, petexp = 0.5, petmoney = 0, useTask = { 2190, 3 }, },
  [6] = { num = 60000, id = { 6, 1, 1493, 1 }, monster = 100, spe = 1, buffid = 1894, petid = 76, petexp = 0.8, petmoney = 0, useTask = { 2190, 4 }, },
  [7] = { num = 70000, id = { 6, 1, 1494, 1 }, monster = 110, spe = 0, buffid = 1895, petid = 77, petexp = 0.8, petmoney = 0, useTask = { 2190, 5 }, },
  [8] = { num = 80000, id = { 6, 1, 1495, 1 }, monster = 110, spe = 0, buffid = 1896, petid = 77, petexp = 0.8, petmoney = 0, useTask = { 2190, 6 }, },
  [9] = { num = 90000, id = { 6, 1, 1496, 1 }, monster = 110, spe = 1, buffid = 1897, petid = 77, petexp = 1, petmoney = 0, useTask = { 2190, 7 }, },
  [10] = { num = 90000, id = { 6, 1, 1497, 1 }, monster = 110, spe = 0, buffid = 1898, petid = 78, petexp = 1, petmoney = 0, useTask = { 2190, 8 }, },
  [11] = { num = 90000, id = { 6, 1, 1660, 1 }, monster = 110, spe = 0, buffid = 2008, petid = 110, petexp = 1, petmoney = 0, useTask = { 2191, 7 }, },
}

PTAP_TaskTable_HuangFeiHu = {
  [1] = { num = 10000, id = { 6, 1, 1498, 1 }, monster = 90, spe = 0, buffid = 1899, petid = 79, petexp = 0.3, petmoney = 0, useTask = { 2190, 9 }, },
  [2] = { num = 20000, id = { 6, 1, 1499, 1 }, monster = 90, spe = 0, buffid = 1900, petid = 79, petexp = 0.4, petmoney = 0, useTask = { 2190, 10 }, },
  [3] = { num = 30000, id = { 6, 1, 1500, 1 }, monster = 90, spe = 1, buffid = 1901, petid = 79, petexp = 0.5, petmoney = 0, useTask = { 2190, 11 }, },
  [4] = { num = 40000, id = { 6, 1, 1501, 1 }, monster = 100, spe = 0, buffid = 1902, petid = 80, petexp = 0.5, petmoney = 0, useTask = { 2190, 12 }, },
  [5] = { num = 50000, id = { 6, 1, 1502, 1 }, monster = 100, spe = 0, buffid = 1903, petid = 80, petexp = 0.5, petmoney = 0, useTask = { 2190, 13 }, },
  [6] = { num = 60000, id = { 6, 1, 1503, 1 }, monster = 100, spe = 1, buffid = 1904, petid = 80, petexp = 0.8, petmoney = 0, useTask = { 2190, 14 }, },
  [7] = { num = 70000, id = { 6, 1, 1504, 1 }, monster = 110, spe = 0, buffid = 1905, petid = 81, petexp = 0.8, petmoney = 0, useTask = { 2190, 15 }, },
  [8] = { num = 80000, id = { 6, 1, 1505, 1 }, monster = 110, spe = 0, buffid = 1906, petid = 81, petexp = 0.8, petmoney = 0, useTask = { 2190, 16 }, },
  [9] = { num = 90000, id = { 6, 1, 1506, 1 }, monster = 110, spe = 1, buffid = 1907, petid = 81, petexp = 1, petmoney = 0, useTask = { 2190, 17 }, },
  [10] = { num = 90000, id = { 6, 1, 1507, 1 }, monster = 110, spe = 0, buffid = 1908, petid = 82, petexp = 1, petmoney = 0, useTask = { 2190, 18 }, },
  [11] = { num = 90000, id = { 6, 1, 1661, 1 }, monster = 110, spe = 0, buffid = 2009, petid = 111, petexp = 1, petmoney = 0, useTask = { 2191, 8 }, },
}

PTAP_PetRoleCombosTask = 2199

PTAP_L_PETCOMBOS = {

  { name = "Trung KhuyÓn Tuú Chñ: D­¬ng TiÔn+Hao Thiªn KhuyÓn", itemID = 1643, bit = 1, buff = 1999, taskIdx = { 2195, 1 }, petID = 107, pet1 = { 2190, 28, 2191, 9 }, pet2 = { 2212, 7, 2191, 10 }, petname1 = "H¹o Thiªn KhuyÓn", petname2 = "D­¬ng TiÔn", info = "Cã kü n¨ng Tu luyÖn Thiªn Giíi: Treo m¸y t¹i Thiªn th­îng cã x¸c suÊt nhËn ®­îc Minh Quang Ng­ng Lé, giíi h¹n mçi ngµy nhËn 1\nMinh Quang Ng­ng Lé t¨ng 1.5 lÇn kinh nghiÖm khi treo m¸y t¹i thiªn th­îng" },
  { name = "TuyÖt §¹i Song KiÒu: §¸t Kû+Hå HØ MÞ", itemID = 1639, bit = 6, buff = 1995, taskIdx = { 2194, 1 }, petID = 103, pet1 = { 2188, 10, 2191, 1 }, pet2 = { 2189, 29, 2191, 6 }, petname1 = "Hå Hû MÞ", petname2 = "§¾c Kû", info = "Cã kü n¨ng Lo¹n Hoa Mª Nh·n: Gi¶m x¸c suÊt bÞ b¹o kÝch vËt lý, ph¸p thuËt 5%" },
  { name = "Y B¸t T­¬ng TruyÒn: Th¸i Êt+Na Tra", itemID = 1640, bit = 2, buff = 1996, taskIdx = { 2194, 2 }, petID = 104, pet1 = { 2188, 20, 2191, 2 }, pet2 = { 2189, 19, 2191, 5 }, petname1 = "Na Tra", petname2 = "Th¸i Êt Ch©n Nh©n", info = "Cã kü n¨ng S­ §å T­¬ng Hç: Sè lÇn Hé chñ +1, t¨ng tèc ®é xuÊt chiªu vò khÝ, ma ph¸p 2%" },
  { name = "§øc Cao Väng Träng: Kh­¬ng Tö Nha+Lý TÞnh", itemID = 1644, bit = 5, buff = 2000, taskIdx = { 2195, 2 }, petID = 108, pet1 = { 2212, 17, 2191, 11 }, pet2 = { 2212, 27, 2191, 12 }, petname1 = "Kh­¬ng Tö Nha", petname2 = "Lý TÞnh", info = "Cã kü n¨ng HiÒn Gi¶ trî uy, t¨ng thêi gian thä th­¬ng 10, s¸t th­¬ng c¬ b¶n +30 ®iÓm" },
  { name = "TuyÖt Gi¸o Tinh NhuÖ: Th¹ch C¬+Th©n C«ng B¸o", itemID = 1641, bit = 3, buff = 1997, taskIdx = { 2194, 3 }, petID = 105, pet1 = { 2189, 9, 2191, 4 }, pet2 = { 2190, 8, 2191, 7 }, petname1 = "Th¹ch C¬ N­¬ng N­¬ng", petname2 = "Th©n C«ng B¸o", info = "Cã kü n¨ng ThuËt ch¹y trèn: TÊt c¶ kh¸ng tÝnh +5%, Gi¶m thêi gian thä th­¬ng 25 ®iÓm" },
  { name = "ChÝnh NghÜa Chi §¹o: Hoµng Phi Hæ+L«i ChÊn Tö", itemID = 1642, bit = 4, buff = 1998, taskIdx = { 2194, 4 }, petID = 106, pet1 = { 2188, 30, 2191, 3 }, pet2 = { 2190, 18, 2191, 8 }, petname1 = "L«i ChÊn Tö", petname2 = "Hoµng Phi Hæ", info = "Cã kü n¨ng NhÊt Th©n ChÝnh KhÝ: ChÞu S¸t th­¬ng c¬ b¶n, s¸t th­¬ng PhÊp thuËt gi¶m 5%" },

}

PTAP_TaskTable_NewAllPet = {
  [1] = { petname = "H¹o Thiªn KhuyÓn", taskvalue = { 2200, 2201, 2202 }, PetType = 9, taskNoteIdx = { 2072, 2073, 2074, 2075 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Hao Thiªn KhuyÓn<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn CÈu TÇm VËt]<c> kinh nghiÖm nhiÖm vô Hoa ThÇn bÝ t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Trung KhuyÓn Trî ChiÕn]<c> cã x¸c suÊt t¨ng 30% s¸t th­¬ng lªn qu¸i, duy tr× 5s", onlineday = 201709,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1599, 1 }, monster = 90, spe = 0, buffid = 1955, petid = 87, petexp = 0.3, petmoney = 0, useTask = { 2190, 19 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1600, 1 }, monster = 90, spe = 0, buffid = 1956, petid = 87, petexp = 0.4, petmoney = 0, useTask = { 2190, 20 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1601, 1 }, monster = 90, spe = 1, buffid = 1957, petid = 87, petexp = 0.5, petmoney = 0, useTask = { 2190, 21 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1602, 1 }, monster = 100, spe = 0, buffid = 1958, petid = 88, petexp = 0.5, petmoney = 0, useTask = { 2190, 22 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1603, 1 }, monster = 100, spe = 0, buffid = 1959, petid = 88, petexp = 0.5, petmoney = 0, useTask = { 2190, 23 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1604, 1 }, monster = 100, spe = 1, buffid = 1960, petid = 88, petexp = 0.8, petmoney = 0, useTask = { 2190, 24 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1605, 1 }, monster = 110, spe = 0, buffid = 1961, petid = 89, petexp = 0.8, petmoney = 0, useTask = { 2190, 25 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1606, 1 }, monster = 110, spe = 0, buffid = 1962, petid = 89, petexp = 0.8, petmoney = 0, useTask = { 2190, 26 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1607, 1 }, monster = 110, spe = 1, buffid = 1963, petid = 89, petexp = 1, petmoney = 0, useTask = { 2190, 27 }, lvl = 9, },
            [10] = { num = 90000, id = { 6, 1, 1608, 1 }, monster = 110, spe = 0, buffid = 1964, petid = 90, petexp = 1, petmoney = 0, useTask = { 2190, 28 }, lvl = 10, },
            [11] = { num = 90000, id = { 6, 1, 1663, 1 }, monster = 110, spe = 0, buffid = 2010, petid = 112, petexp = 1, petmoney = 0, useTask = { 2191, 9 }, lvl = 11, itemname = "Kim Th©n Hao Thiªn KhuyÓn" },
          },
  },
  [2] = { petname = "D­¬ng TiÔn", taskvalue = { 2203, 2204, 2205 }, PetType = 10, taskNoteIdx = { 2076, 2077, 2078, 2079 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>D­¬ng TiÔn<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Uy Phong LÉm LiÖt]<c> kinh nghiÖm nhiÖm vô dÑp lo¹n V¹n Tiªn TrËn t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Thiªn Nh·n Phóc X¹]<c> tu vi nhiÖm vô H« Tiªn Ho¸n Ma t¨ng 100%", onlineday = 201709,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1609, 1 }, monster = 90, spe = 0, buffid = 1965, petid = 91, petexp = 0.3, petmoney = 0, useTask = { 2190, 29 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1610, 1 }, monster = 90, spe = 0, buffid = 1966, petid = 91, petexp = 0.4, petmoney = 0, useTask = { 2190, 30 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1611, 1 }, monster = 90, spe = 1, buffid = 1967, petid = 91, petexp = 0.5, petmoney = 0, useTask = { 2190, 31 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1612, 1 }, monster = 100, spe = 0, buffid = 1968, petid = 92, petexp = 0.5, petmoney = 0, useTask = { 2212, 1 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1613, 1 }, monster = 100, spe = 0, buffid = 1969, petid = 92, petexp = 0.5, petmoney = 0, useTask = { 2212, 2 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1614, 1 }, monster = 100, spe = 1, buffid = 1970, petid = 92, petexp = 0.8, petmoney = 0, useTask = { 2212, 3 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1615, 1 }, monster = 110, spe = 0, buffid = 1971, petid = 93, petexp = 0.8, petmoney = 0, useTask = { 2212, 4 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1616, 1 }, monster = 110, spe = 0, buffid = 1972, petid = 93, petexp = 0.8, petmoney = 0, useTask = { 2212, 5 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1617, 1 }, monster = 110, spe = 1, buffid = 1973, petid = 93, petexp = 1, petmoney = 0, useTask = { 2212, 6 }, lvl = 9, },
            [10] = { num = 90000, id = { 6, 1, 1618, 1 }, monster = 110, spe = 0, buffid = 1974, petid = 94, petexp = 1, petmoney = 0, useTask = { 2212, 7 }, lvl = 10, },
            [11] = { num = 90000, id = { 6, 1, 1664, 1 }, monster = 110, spe = 0, buffid = 2011, petid = 113, petexp = 1, petmoney = 0, useTask = { 2191, 10 }, lvl = 11, itemname = "Kim ThÇn D­¬ng TiÔn" },
          },
  },
  [3] = { petname = "Kh­¬ng Tö Nha", taskvalue = { 2206, 2207, 2208 }, PetType = 11, taskNoteIdx = { 2080, 2081, 2082, 2083 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Kh­¬ng Tö Nha<c> cã nh÷ng kü n¨ng sau:\n<c=y>[NguyÖn Gi¶ Th­îng C©u]<c> kinh nghiÖm nhiÖm vô LÝnh §¸nh Thuª t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Tiªn Nh©n ChØ Lé]<c> lóc hµng phôc ThËp TuyÖt Thiªn Qu©n vµ Th«ng Thiªn Gi¸o Chñ cã x¸c suÊt ®¸nh r¬i ra qu¸i phï", onlineday = 201712,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1619, 1 }, monster = 90, spe = 0, buffid = 1975, petid = 95, petexp = 0.3, petmoney = 0, useTask = { 2212, 8 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1620, 1 }, monster = 90, spe = 0, buffid = 1976, petid = 95, petexp = 0.4, petmoney = 0, useTask = { 2212, 9 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1621, 1 }, monster = 90, spe = 1, buffid = 1977, petid = 95, petexp = 0.5, petmoney = 0, useTask = { 2212, 10 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1622, 1 }, monster = 100, spe = 0, buffid = 1978, petid = 96, petexp = 0.5, petmoney = 0, useTask = { 2212, 11 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1623, 1 }, monster = 100, spe = 0, buffid = 1979, petid = 96, petexp = 0.5, petmoney = 0, useTask = { 2212, 12 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1624, 1 }, monster = 100, spe = 1, buffid = 1980, petid = 96, petexp = 0.8, petmoney = 0, useTask = { 2212, 13 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1625, 1 }, monster = 110, spe = 0, buffid = 1981, petid = 97, petexp = 0.8, petmoney = 0, useTask = { 2212, 14 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1626, 1 }, monster = 110, spe = 0, buffid = 1982, petid = 97, petexp = 0.8, petmoney = 0, useTask = { 2212, 15 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1627, 1 }, monster = 110, spe = 1, buffid = 1983, petid = 97, petexp = 1, petmoney = 0, useTask = { 2212, 16 }, lvl = 9, },
            [10] = { num = 90000, id = { 6, 1, 1628, 1 }, monster = 110, spe = 0, buffid = 1984, petid = 98, petexp = 1, petmoney = 0, useTask = { 2212, 17 }, lvl = 10, },
            [11] = { num = 90000, id = { 6, 1, 1665, 1 }, monster = 110, spe = 0, buffid = 2012, petid = 114, petexp = 1, petmoney = 0, useTask = { 2191, 11 }, lvl = 11, itemname = "Kim Th©n Kh­¬ng Tö Nha" },
          },
  },
  [4] = { petname = "Lý TÞnh", taskvalue = { 2209, 2210, 2211 }, PetType = 12, taskNoteIdx = { 2084, 2085, 2086, 2087 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Lý TÞnh<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn V­¬ng Chi Né]<c> kinh nghiÖm nhiÖm vô HÊp Hån ¢m S¸t t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[B¶o Th¸p TrÊn §Þch]<c> cã x¸c suÊt g©y ®Þnh th©n 2s", onlineday = 201803,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1629, 1 }, monster = 90, spe = 0, buffid = 1985, petid = 99, petexp = 0.3, petmoney = 0, useTask = { 2212, 18 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1630, 1 }, monster = 90, spe = 0, buffid = 1986, petid = 99, petexp = 0.4, petmoney = 0, useTask = { 2212, 19 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1631, 1 }, monster = 90, spe = 1, buffid = 1987, petid = 99, petexp = 0.5, petmoney = 0, useTask = { 2212, 20 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1632, 1 }, monster = 100, spe = 0, buffid = 1988, petid = 100, petexp = 0.5, petmoney = 0, useTask = { 2212, 21 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1633, 1 }, monster = 100, spe = 0, buffid = 1989, petid = 100, petexp = 0.5, petmoney = 0, useTask = { 2212, 22 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1634, 1 }, monster = 100, spe = 1, buffid = 1990, petid = 100, petexp = 0.8, petmoney = 0, useTask = { 2212, 23 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1635, 1 }, monster = 110, spe = 0, buffid = 1991, petid = 101, petexp = 0.8, petmoney = 0, useTask = { 2212, 24 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1636, 1 }, monster = 110, spe = 0, buffid = 1992, petid = 101, petexp = 0.8, petmoney = 0, useTask = { 2212, 25 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1637, 1 }, monster = 110, spe = 1, buffid = 1993, petid = 101, petexp = 1, petmoney = 0, useTask = { 2212, 26 }, lvl = 9, },
            [10] = { num = 90000, id = { 6, 1, 1638, 1 }, monster = 110, spe = 0, buffid = 1994, petid = 102, petexp = 1, petmoney = 0, useTask = { 2212, 27 }, lvl = 10, },
            [11] = { num = 90000, id = { 6, 1, 1735, 1 }, monster = 110, spe = 0, buffid = 2058, petid = 127, petexp = 1, petmoney = 0, useTask = { 2191, 12 }, lvl = 11, itemname = "Kim Th¸p Lý TÞnh" },
          },
  },
  [5] = { petname = "Phi Th¨ng-Hå HØ MÞ", taskvalue = { 2230, 2231, 2232 }, PetType = 13, taskNoteIdx = { 2088, 2089, 2090, 2091 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Hå HØ MÞ<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Phi Tö TiÕu]<c>: kinh nghiÖm nhiÖm vô VËn L­¬ng t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ dïng ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Hång Nhan Tuý<c>: cã x¸c suÊt khiÕn kÎ ®Þch trong 5 gi©y gi¶m kh¶ n¨ng håi phôc ®i 30%", onlineday = 201806,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1675, 1 }, monster = 90, spe = 0, buffid = 2019, petid = 115, petexp = 0.3, petmoney = 0, useTask = { 2212, 28 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1676, 1 }, monster = 90, spe = 0, buffid = 2020, petid = 115, petexp = 0.4, petmoney = 0, useTask = { 2212, 29 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1677, 1 }, monster = 90, spe = 1, buffid = 2021, petid = 115, petexp = 0.5, petmoney = 0, useTask = { 2212, 30 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1678, 1 }, monster = 100, spe = 0, buffid = 2022, petid = 116, petexp = 0.5, petmoney = 0, useTask = { 2212, 31 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1679, 1 }, monster = 100, spe = 0, buffid = 2023, petid = 116, petexp = 0.5, petmoney = 0, useTask = { 2233, 1 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1680, 1 }, monster = 100, spe = 1, buffid = 2024, petid = 116, petexp = 0.8, petmoney = 0, useTask = { 2233, 2 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1681, 1 }, monster = 110, spe = 0, buffid = 2025, petid = 117, petexp = 0.8, petmoney = 0, useTask = { 2233, 3 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1682, 1 }, monster = 110, spe = 0, buffid = 2026, petid = 117, petexp = 0.8, petmoney = 0, useTask = { 2233, 4 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1683, 1 }, monster = 110, spe = 1, buffid = 2027, petid = 117, petexp = 1, petmoney = 0, useTask = { 2233, 5 }, lvl = 9, },
            [10] = { num = 90000, id = { 6, 1, 1684, 1 }, monster = 110, spe = 0, buffid = 2028, petid = 118, petexp = 1, petmoney = 0, useTask = { 2233, 6 }, lvl = 10, },
            [11] = { num = 90000, id = { 6, 1, 1736, 1 }, monster = 110, spe = 0, buffid = 2059, petid = 128, petexp = 1, petmoney = 0, useTask = { 2191, 13 }, lvl = 11, itemname = "Kim Kª Phi Th¨ng Hå HØ MÞ" },
          },
  },
  [6] = { petname = "Phi Th¨ng-Na Tra", taskvalue = { 2234, 2235, 2236 }, PetType = 14, taskNoteIdx = { 2092, 2093, 2094, 2095 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Na Tra<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Linh Ch©u Tö]<c>: kinh nghiÖm nhiÖm vô Thiªn Tµi §Þa B¶o t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ dïng ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Lùc Cµn Kh«n]<c> x¸c suÊt khiÕn kÎ ®Þch chÞu lùc Cµn Kh«n, gióp b¶n th©n vµ tæ ®éi t¨ng b¹o kÝch.", onlineday = 201809,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1687, 1 }, monster = 90, spe = 0, buffid = 2034, petid = 119, petexp = 0.3, petmoney = 0, useTask = { 2233, 7 }, lvl = 1 },
            [2] = { num = 20000, id = { 6, 1, 1688, 1 }, monster = 90, spe = 0, buffid = 2035, petid = 119, petexp = 0.4, petmoney = 0, useTask = { 2233, 8 }, lvl = 2 },
            [3] = { num = 30000, id = { 6, 1, 1689, 1 }, monster = 90, spe = 1, buffid = 2036, petid = 119, petexp = 0.5, petmoney = 0, useTask = { 2233, 9 }, lvl = 3 },
            [4] = { num = 40000, id = { 6, 1, 1690, 1 }, monster = 100, spe = 0, buffid = 2037, petid = 120, petexp = 0.5, petmoney = 0, useTask = { 2233, 10 }, lvl = 4 },
            [5] = { num = 50000, id = { 6, 1, 1691, 1 }, monster = 100, spe = 0, buffid = 2038, petid = 120, petexp = 0.5, petmoney = 0, useTask = { 2233, 11 }, lvl = 5 },
            [6] = { num = 60000, id = { 6, 1, 1692, 1 }, monster = 100, spe = 1, buffid = 2039, petid = 120, petexp = 0.8, petmoney = 0, useTask = { 2233, 12 }, lvl = 6 },
            [7] = { num = 70000, id = { 6, 1, 1693, 1 }, monster = 110, spe = 0, buffid = 2040, petid = 121, petexp = 0.8, petmoney = 0, useTask = { 2233, 13 }, lvl = 7 },
            [8] = { num = 80000, id = { 6, 1, 1694, 1 }, monster = 110, spe = 0, buffid = 2041, petid = 121, petexp = 0.8, petmoney = 0, useTask = { 2233, 14 }, lvl = 8 },
            [9] = { num = 90000, id = { 6, 1, 1695, 1 }, monster = 110, spe = 1, buffid = 2042, petid = 121, petexp = 1, petmoney = 0, useTask = { 2233, 15 }, lvl = 9 },
            [10] = { num = 90000, id = { 6, 1, 1696, 1 }, monster = 110, spe = 0, buffid = 2043, petid = 122, petexp = 1, petmoney = 0, useTask = { 2233, 16 }, lvl = 10 },
            [11] = { num = 90000, id = { 6, 1, 1737, 1 }, monster = 110, spe = 0, buffid = 2060, petid = 129, petexp = 1, petmoney = 0, useTask = { 2191, 14 }, lvl = 11, itemname = "Kim Th©n Phi Th¨ng Na Tra" },
          }
  },
  [7] = { petname = "Phi Th¨ng-L«i ChÊn Tö", taskvalue = { 2238, 2239, 2240 }, PetType = 15, taskNoteIdx = { 2096, 2097, 2098, 2099 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>L«i ChÊn Tö<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn §Þa Linh KhÝ]<c>: kinh nghiÖm nhiÖm vô Tø T­îng Linh Tª t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ dïng ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[L«i §×nh Chóc Phóc]<c> bÞ tÊn c«ng cã x¸c suÊt gióp toµn tæ ®éi (trªn cïng b¶n ®å) t¨ng sinh lùc tèi ®a = sinh lùc tèi ®a cña ng­êi ch¬i mang theo Phi th¨ng-L«i ChÊn Tö!", onlineday = 201812,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1724, 1 }, monster = 90, spe = 0, buffid = 2048, petid = 123, petexp = 0.3, petmoney = 0, useTask = { 2233, 17 }, lvl = 1 },
            [2] = { num = 20000, id = { 6, 1, 1725, 1 }, monster = 90, spe = 0, buffid = 2049, petid = 123, petexp = 0.4, petmoney = 0, useTask = { 2233, 18 }, lvl = 2 },
            [3] = { num = 30000, id = { 6, 1, 1726, 1 }, monster = 90, spe = 1, buffid = 2050, petid = 123, petexp = 0.5, petmoney = 0, useTask = { 2233, 19 }, lvl = 3 },
            [4] = { num = 40000, id = { 6, 1, 1727, 1 }, monster = 100, spe = 0, buffid = 2051, petid = 124, petexp = 0.5, petmoney = 0, useTask = { 2233, 20 }, lvl = 4 },
            [5] = { num = 50000, id = { 6, 1, 1728, 1 }, monster = 100, spe = 0, buffid = 2052, petid = 124, petexp = 0.5, petmoney = 0, useTask = { 2233, 21 }, lvl = 5 },
            [6] = { num = 60000, id = { 6, 1, 1729, 1 }, monster = 100, spe = 1, buffid = 2053, petid = 124, petexp = 0.8, petmoney = 0, useTask = { 2233, 22 }, lvl = 6 },
            [7] = { num = 70000, id = { 6, 1, 1730, 1 }, monster = 110, spe = 0, buffid = 2054, petid = 125, petexp = 0.8, petmoney = 0, useTask = { 2233, 23 }, lvl = 7 },
            [8] = { num = 80000, id = { 6, 1, 1731, 1 }, monster = 110, spe = 0, buffid = 2055, petid = 125, petexp = 0.8, petmoney = 0, useTask = { 2233, 24 }, lvl = 8 },
            [9] = { num = 90000, id = { 6, 1, 1732, 1 }, monster = 110, spe = 1, buffid = 2056, petid = 125, petexp = 1, petmoney = 0, useTask = { 2233, 25 }, lvl = 9 },
            [10] = { num = 90000, id = { 6, 1, 1733, 1 }, monster = 110, spe = 0, buffid = 2057, petid = 126, petexp = 1, petmoney = 0, useTask = { 2233, 26 }, lvl = 10 },
            [11] = { num = 90000, id = { 6, 1, 1738, 1 }, monster = 110, spe = 0, buffid = 2061, petid = 130, petexp = 1, petmoney = 0, useTask = { 2191, 15 }, lvl = 11, itemname = "Kim Vò Phi Th¨ng L«i ChÊn Tö" },
          },
  },
  [8] = { petname = "Phi Th¨ng-Th¹ch C¬", taskvalue = { 2246, 2247, 2248 }, PetType = 16, taskNoteIdx = { 2100, 2101, 2102, 2103 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Th¹ch C¬<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn Nhiªn Chi Lùc]<c> kinh nghiÖm nhiÖm vô Thiªn §×nh ThÇn Thô t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ dïng ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[V©n Quang Tr¸o<c> lóc tÊn c«ng cã tØ lÖ toµn tæ ®éi (cïng b¶n ®å) nhËn ®­îc tr¹ng th¸i [V©n Quang Tr¸o]: Ph¶n ®ßn tÇm xa 7%, Ph¶n ®ßn cËn chiÕn 7%, tr¹ng th¸i kÐo dµi 3s, håi khÝ 15s!", onlineday = 201903,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1747, 1 }, monster = 90, spe = 0, buffid = 2069, petid = 131, petexp = 0.3, petmoney = 0, useTask = { 2233, 27 }, lvl = 1 },
            [2] = { num = 20000, id = { 6, 1, 1748, 1 }, monster = 90, spe = 0, buffid = 2070, petid = 131, petexp = 0.4, petmoney = 0, useTask = { 2233, 28 }, lvl = 2 },
            [3] = { num = 30000, id = { 6, 1, 1749, 1 }, monster = 90, spe = 1, buffid = 2071, petid = 131, petexp = 0.5, petmoney = 0, useTask = { 2233, 29 }, lvl = 3 },
            [4] = { num = 40000, id = { 6, 1, 1750, 1 }, monster = 100, spe = 0, buffid = 2072, petid = 132, petexp = 0.5, petmoney = 0, useTask = { 2233, 30 }, lvl = 4 },
            [5] = { num = 50000, id = { 6, 1, 1751, 1 }, monster = 100, spe = 0, buffid = 2073, petid = 132, petexp = 0.5, petmoney = 0, useTask = { 2233, 31 }, lvl = 5 },
            [6] = { num = 60000, id = { 6, 1, 1752, 1 }, monster = 100, spe = 1, buffid = 2074, petid = 132, petexp = 0.8, petmoney = 0, useTask = { 2213, 1 }, lvl = 6 },
            [7] = { num = 70000, id = { 6, 1, 1753, 1 }, monster = 110, spe = 0, buffid = 2075, petid = 133, petexp = 0.8, petmoney = 0, useTask = { 2213, 2 }, lvl = 7 },
            [8] = { num = 80000, id = { 6, 1, 1754, 1 }, monster = 110, spe = 0, buffid = 2076, petid = 133, petexp = 0.8, petmoney = 0, useTask = { 2213, 3 }, lvl = 8 },
            [9] = { num = 90000, id = { 6, 1, 1755, 1 }, monster = 110, spe = 1, buffid = 2077, petid = 133, petexp = 1, petmoney = 0, useTask = { 2213, 4 }, lvl = 9 },
            [10] = { num = 90000, id = { 6, 1, 1756, 1 }, monster = 110, spe = 0, buffid = 2078, petid = 134, petexp = 1, petmoney = 0, useTask = { 2213, 5 }, lvl = 10 },
            [11] = { num = 90000, id = { 6, 1, 1807, 1 }, monster = 110, spe = 0, buffid = 2124, petid = 147, petexp = 1, petmoney = 0, useTask = { 2191, 16 }, lvl = 11, itemname = "Kim Quan Phi Th¨ng Th¹ch C¬" },
          },
  },

  [9] = { petname = "Phi Th¨ng-Th¸i Êt", taskvalue = { 2249, 2250, 2251 }, PetType = 17, taskNoteIdx = { 2104, 2105, 2106, 2107 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Th¸i Êt<c>: \n<c=y>[Tu luyÖn §¾c §¹o]<c>kinh nghiÖm nhiÖm vô thÝ luyÖn ThÊt Qu¶i t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Tô Lý Cµn Kh«n]<c> hoµn thµnh nhiÖm vô Tiªn Ma danh väng nhËn ®­îc t¨ng 100%.", onlineday = 201906,
          task = {
            [1] = { num = 10000, id = { 6, 1, 1758, 1 }, monster = 90, spe = 0, buffid = 2081, petid = 135, petexp = 0.3, petmoney = 0, useTask = { 2213, 6 }, lvl = 1 },
            [2] = { num = 20000, id = { 6, 1, 1759, 1 }, monster = 90, spe = 0, buffid = 2082, petid = 135, petexp = 0.4, petmoney = 0, useTask = { 2213, 7 }, lvl = 2 },
            [3] = { num = 30000, id = { 6, 1, 1760, 1 }, monster = 90, spe = 1, buffid = 2083, petid = 135, petexp = 0.5, petmoney = 0, useTask = { 2213, 8 }, lvl = 3 },
            [4] = { num = 40000, id = { 6, 1, 1761, 1 }, monster = 100, spe = 0, buffid = 2084, petid = 136, petexp = 0.5, petmoney = 0, useTask = { 2213, 9 }, lvl = 4 },
            [5] = { num = 50000, id = { 6, 1, 1762, 1 }, monster = 100, spe = 0, buffid = 2085, petid = 136, petexp = 0.5, petmoney = 0, useTask = { 2213, 10 }, lvl = 5 },
            [6] = { num = 60000, id = { 6, 1, 1763, 1 }, monster = 100, spe = 1, buffid = 2086, petid = 136, petexp = 0.8, petmoney = 0, useTask = { 2213, 11 }, lvl = 6 },
            [7] = { num = 70000, id = { 6, 1, 1764, 1 }, monster = 110, spe = 0, buffid = 2087, petid = 137, petexp = 0.8, petmoney = 0, useTask = { 2213, 12 }, lvl = 7 },
            [8] = { num = 80000, id = { 6, 1, 1765, 1 }, monster = 110, spe = 0, buffid = 2088, petid = 137, petexp = 0.8, petmoney = 0, useTask = { 2213, 13 }, lvl = 8 },
            [9] = { num = 90000, id = { 6, 1, 1766, 1 }, monster = 110, spe = 1, buffid = 2089, petid = 137, petexp = 1, petmoney = 0, useTask = { 2213, 14 }, lvl = 9 },
            [10] = { num = 90000, id = { 6, 1, 1767, 1 }, monster = 110, spe = 0, buffid = 2090, petid = 138, petexp = 1, petmoney = 0, useTask = { 2213, 15 }, lvl = 10 },
            [11] = { num = 90000, id = { 6, 1, 1808, 1 }, monster = 110, spe = 0, buffid = 2125, petid = 148, petexp = 1, petmoney = 0, useTask = { 2191, 17 }, lvl = 11, itemname = "Kim Tiªn Phi Th¨ng Th¸i Êt" },
          },
  },

  [10] = { petname = "Phi Th¨ng-§¸t Kû", taskvalue = { 2260, 2261, 2262 }, PetType = 18, taskNoteIdx = { 2108, 2109, 2110, 2111 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>§¸t Kû<c>: \n<c=y>[Tu luyÖn §¾c §¹o]<c> kinh nghiÖm nhiÖm vô thu thËp ®¹o cô t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[MÞ HoÆc]<c> lóc chiÕn ®Êu cã x¸c suÊt khiÕn ®Þch bÞ hçn lo¹n 5 gi©y, b¶n th©n vµ ®ång ®éi nhËn 1% b¹o kÝch, duy tr× 5 gi©y, håi khÝ 30 gi©y.", onlineday = 201909,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1776, 1 }, monster = 90, spe = 0, buffid = 2097, petid = 139, petexp = 0.3, petmoney = 0, useTask = { 2213, 16 }, lvl = 1 },
             [2] = { num = 20000, id = { 6, 1, 1777, 1 }, monster = 90, spe = 0, buffid = 2098, petid = 139, petexp = 0.4, petmoney = 0, useTask = { 2213, 17 }, lvl = 2 },
             [3] = { num = 30000, id = { 6, 1, 1778, 1 }, monster = 90, spe = 1, buffid = 2099, petid = 139, petexp = 0.5, petmoney = 0, useTask = { 2213, 18 }, lvl = 3 },
             [4] = { num = 40000, id = { 6, 1, 1779, 1 }, monster = 100, spe = 0, buffid = 2100, petid = 140, petexp = 0.5, petmoney = 0, useTask = { 2213, 19 }, lvl = 4 },
             [5] = { num = 50000, id = { 6, 1, 1780, 1 }, monster = 100, spe = 0, buffid = 2101, petid = 140, petexp = 0.5, petmoney = 0, useTask = { 2213, 20 }, lvl = 5 },
             [6] = { num = 60000, id = { 6, 1, 1781, 1 }, monster = 100, spe = 1, buffid = 2102, petid = 140, petexp = 0.8, petmoney = 0, useTask = { 2213, 21 }, lvl = 6 },
             [7] = { num = 70000, id = { 6, 1, 1782, 1 }, monster = 110, spe = 0, buffid = 2103, petid = 141, petexp = 0.8, petmoney = 0, useTask = { 2213, 22 }, lvl = 7 },
             [8] = { num = 80000, id = { 6, 1, 1783, 1 }, monster = 110, spe = 0, buffid = 2104, petid = 141, petexp = 0.8, petmoney = 0, useTask = { 2213, 23 }, lvl = 8 },
             [9] = { num = 90000, id = { 6, 1, 1784, 1 }, monster = 110, spe = 1, buffid = 2105, petid = 141, petexp = 1, petmoney = 0, useTask = { 2213, 24 }, lvl = 9 },
             [10] = { num = 90000, id = { 6, 1, 1785, 1 }, monster = 110, spe = 0, buffid = 2106, petid = 142, petexp = 1, petmoney = 0, useTask = { 2213, 25 }, lvl = 10 },
             [11] = { num = 90000, id = { 6, 1, 1809, 1 }, monster = 110, spe = 0, buffid = 2126, petid = 149, petexp = 1, petmoney = 0, useTask = { 2191, 18 }, lvl = 11, itemname = "Kim Tiªn Phi Th¨ng §¸t Kû" },
           },
  },

  [11] = { petname = "Phi Th¨ng-Th©n C«ng B¸o", taskvalue = { 2264, 2265, 2266 }, PetType = 19, taskNoteIdx = { 2112, 2113, 2114, 2115 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Th©n C«ng B¸o<c>: \n<c=y>[B¸t DiÖn Linh Lung]<c> kinh nghiÖm nhiÖm vô th¨m dß t×nh b¸o t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[X¶o ThiÖt Nh­ Hoµng]<c> hoµn thµnh nhiÖm vô LiÖp M· Th­ëng Kim nhËn ®­îc thªm 5~ 8 c¸i M¶nh Phï Th¹ch(Kho¸).", onlineday = 201912,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1789, 1 }, monster = 90, spe = 0, buffid = 2108, petid = 143, petexp = 0.3, petmoney = 0, useTask = { 2213, 26 }, lvl = 1 },
             [2] = { num = 20000, id = { 6, 1, 1790, 1 }, monster = 90, spe = 0, buffid = 2109, petid = 143, petexp = 0.4, petmoney = 0, useTask = { 2213, 27 }, lvl = 2 },
             [3] = { num = 30000, id = { 6, 1, 1791, 1 }, monster = 90, spe = 1, buffid = 2110, petid = 143, petexp = 0.5, petmoney = 0, useTask = { 2213, 28 }, lvl = 3 },
             [4] = { num = 40000, id = { 6, 1, 1792, 1 }, monster = 100, spe = 0, buffid = 2111, petid = 144, petexp = 0.5, petmoney = 0, useTask = { 2213, 29 }, lvl = 4 },
             [5] = { num = 50000, id = { 6, 1, 1793, 1 }, monster = 100, spe = 0, buffid = 2112, petid = 144, petexp = 0.5, petmoney = 0, useTask = { 2213, 30 }, lvl = 5 },
             [6] = { num = 60000, id = { 6, 1, 1794, 1 }, monster = 100, spe = 1, buffid = 2113, petid = 144, petexp = 0.8, petmoney = 0, useTask = { 2213, 31 }, lvl = 6 },
             [7] = { num = 70000, id = { 6, 1, 1795, 1 }, monster = 110, spe = 0, buffid = 2114, petid = 145, petexp = 0.8, petmoney = 0, useTask = { 2214, 1 }, lvl = 7 },
             [8] = { num = 80000, id = { 6, 1, 1796, 1 }, monster = 110, spe = 0, buffid = 2115, petid = 145, petexp = 0.8, petmoney = 0, useTask = { 2214, 2 }, lvl = 8 },
             [9] = { num = 90000, id = { 6, 1, 1797, 1 }, monster = 110, spe = 1, buffid = 2116, petid = 145, petexp = 1, petmoney = 0, useTask = { 2214, 3 }, lvl = 9 },
             [10] = { num = 90000, id = { 6, 1, 1798, 1 }, monster = 110, spe = 0, buffid = 2117, petid = 146, petexp = 1, petmoney = 0, useTask = { 2214, 4 }, lvl = 10 },
             [11] = { num = 90000, id = { 6, 1, 1810, 1 }, monster = 110, spe = 0, buffid = 2127, petid = 150, petexp = 1, petmoney = 0, useTask = { 2191, 19 }, lvl = 11, itemname = "Kim B¸o Phi Th¨ng Th©n C«ng B¸o" },
           },
  },

  [12] = { petname = "Phi Th¨ng-Hoµng Phi Hæ", taskvalue = { 2267, 2268, 2269 }, PetType = 20, taskNoteIdx = { 2116, 2117, 2118, 2119 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Hoµng Phi Hæ<c>: \n<c=y>[H¹o Nhiªn ChÝnh KhÝ]<c> kinh nghiÖm nhiÖm vô Siªu ®é t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Vò Thµnh V­¬ng Tø Phóc]<c> x2 phÇn th­ëng lÔ tói b¸o danh ngµy", onlineday = 202003,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1811, 1 }, monster = 90, spe = 0, buffid = 2128, petid = 151, petexp = 0.3, petmoney = 0, useTask = { 2214, 5 }, lvl = 1 },
             [2] = { num = 20000, id = { 6, 1, 1812, 1 }, monster = 90, spe = 0, buffid = 2129, petid = 151, petexp = 0.4, petmoney = 0, useTask = { 2214, 6 }, lvl = 2 },
             [3] = { num = 30000, id = { 6, 1, 1813, 1 }, monster = 90, spe = 1, buffid = 2130, petid = 151, petexp = 0.5, petmoney = 0, useTask = { 2214, 7 }, lvl = 3 },
             [4] = { num = 40000, id = { 6, 1, 1814, 1 }, monster = 100, spe = 0, buffid = 2131, petid = 152, petexp = 0.5, petmoney = 0, useTask = { 2214, 8 }, lvl = 4 },
             [5] = { num = 50000, id = { 6, 1, 1815, 1 }, monster = 100, spe = 0, buffid = 2132, petid = 152, petexp = 0.5, petmoney = 0, useTask = { 2214, 9 }, lvl = 5 },
             [6] = { num = 60000, id = { 6, 1, 1816, 1 }, monster = 100, spe = 1, buffid = 2133, petid = 152, petexp = 0.8, petmoney = 0, useTask = { 2214, 10 }, lvl = 6 },
             [7] = { num = 70000, id = { 6, 1, 1817, 1 }, monster = 110, spe = 0, buffid = 2134, petid = 153, petexp = 0.8, petmoney = 0, useTask = { 2214, 11 }, lvl = 7 },
             [8] = { num = 80000, id = { 6, 1, 1818, 1 }, monster = 110, spe = 0, buffid = 2135, petid = 153, petexp = 0.8, petmoney = 0, useTask = { 2214, 12 }, lvl = 8 },
             [9] = { num = 90000, id = { 6, 1, 1819, 1 }, monster = 110, spe = 1, buffid = 2136, petid = 153, petexp = 1, petmoney = 0, useTask = { 2214, 13 }, lvl = 9 },
             [10] = { num = 90000, id = { 6, 1, 1820, 1 }, monster = 110, spe = 0, buffid = 2137, petid = 154, petexp = 1, petmoney = 0, useTask = { 2214, 14 }, lvl = 10 },
             [11] = { num = 90000, id = { 6, 1, 1859, 1 }, monster = 110, spe = 0, buffid = 2177, petid = 167, petexp = 1, petmoney = 0, useTask = { 2191, 20 }, lvl = 11, itemname = "Kim §¶m Phi Th¨ng Hoµng Phi Hæ" },
           },
  },
  [13] = { petname = "Phi Th¨ng-Hao Thiªn KhuyÓn", taskvalue = { 2270, 2271, 2272 }, PetType = 21, taskNoteIdx = { 2072, 2073, 2074, 2120 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Hao Thiªn KhuyÓn<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn CÈu TÇm VËt]<c> kinh nghiÖm nhiÖm vô Hoa ThÇn bÝ t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[D· Ngo¹i TÇm B¶o]<c> ®em theo Hao Thiªn KhuyÓn khi dïng Di Ngo¹i Phï cã x¸c suÊt nhËn ®­îc phÇn th­ëng bÊt ngê.", onlineday = 202006,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1822, 1 }, monster = 90, spe = 0, buffid = 2138, petid = 155, petexp = 0.3, petmoney = 0, useTask = { 2214, 15 }, lvl = 1, },
             [2] = { num = 20000, id = { 6, 1, 1823, 1 }, monster = 90, spe = 0, buffid = 2139, petid = 155, petexp = 0.4, petmoney = 0, useTask = { 2214, 16 }, lvl = 2, },
             [3] = { num = 30000, id = { 6, 1, 1824, 1 }, monster = 90, spe = 1, buffid = 2140, petid = 155, petexp = 0.5, petmoney = 0, useTask = { 2214, 17 }, lvl = 3, },
             [4] = { num = 40000, id = { 6, 1, 1825, 1 }, monster = 100, spe = 0, buffid = 2141, petid = 156, petexp = 0.5, petmoney = 0, useTask = { 2214, 18 }, lvl = 4, },
             [5] = { num = 50000, id = { 6, 1, 1826, 1 }, monster = 100, spe = 0, buffid = 2142, petid = 156, petexp = 0.5, petmoney = 0, useTask = { 2214, 19 }, lvl = 5, },
             [6] = { num = 60000, id = { 6, 1, 1827, 1 }, monster = 100, spe = 1, buffid = 2143, petid = 156, petexp = 0.8, petmoney = 0, useTask = { 2214, 20 }, lvl = 6, },
             [7] = { num = 70000, id = { 6, 1, 1828, 1 }, monster = 110, spe = 0, buffid = 2144, petid = 157, petexp = 0.8, petmoney = 0, useTask = { 2214, 21 }, lvl = 7, },
             [8] = { num = 80000, id = { 6, 1, 1829, 1 }, monster = 110, spe = 0, buffid = 2145, petid = 157, petexp = 0.8, petmoney = 0, useTask = { 2214, 22 }, lvl = 8, },
             [9] = { num = 90000, id = { 6, 1, 1830, 1 }, monster = 110, spe = 1, buffid = 2146, petid = 157, petexp = 1, petmoney = 0, useTask = { 2214, 23 }, lvl = 9, },
             [10] = { num = 90000, id = { 6, 1, 1831, 1 }, monster = 110, spe = 0, buffid = 2147, petid = 158, petexp = 1, petmoney = 0, useTask = { 2214, 24 }, lvl = 10, },
             [11] = { num = 90000, id = { 6, 1, 1860, 1 }, monster = 110, spe = 0, buffid = 2178, petid = 168, petexp = 1, petmoney = 0, useTask = { 2191, 21 }, lvl = 11, itemname = "Kim Th©n Phi Th¨ng Hao Thiªn KhuyÓn" },
           },
  },
  [14] = { petname = "Phi Th¨ng-D­¬ng TiÔn", taskvalue = { 2274, 2275, 2276 }, PetType = 22, taskNoteIdx = { 2076, 2077, 2078, 2121 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>D­¬ng TiÔn<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Uy Phong LÉm LiÖt]<c> kinh nghiÖm nhiÖm vô dÑp lo¹n V¹n Tiªn TrËn t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Thiªn Nh·n Th¸c §å]<c> khi cã D­¬ng TiÔn theo sau lóc diÖt Bµn Cæ hoÆc §¹i §iªu, cã x¸c suÊt nhËn ®­îc ®å phæ Cam hoÆc s¸ch phèi ph­¬ng kü n¨ng sèng.", onlineday = 202009,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1836, 1 }, monster = 90, spe = 0, buffid = 2149, petid = 159, petexp = 0.3, petmoney = 0, useTask = { 2214, 25 }, lvl = 1, },
             [2] = { num = 20000, id = { 6, 1, 1837, 1 }, monster = 90, spe = 0, buffid = 2150, petid = 159, petexp = 0.4, petmoney = 0, useTask = { 2214, 26 }, lvl = 2, },
             [3] = { num = 30000, id = { 6, 1, 1838, 1 }, monster = 90, spe = 1, buffid = 2151, petid = 159, petexp = 0.5, petmoney = 0, useTask = { 2214, 27 }, lvl = 3, },
             [4] = { num = 40000, id = { 6, 1, 1839, 1 }, monster = 100, spe = 0, buffid = 2152, petid = 160, petexp = 0.5, petmoney = 0, useTask = { 2214, 28 }, lvl = 4, },
             [5] = { num = 50000, id = { 6, 1, 1840, 1 }, monster = 100, spe = 0, buffid = 2153, petid = 160, petexp = 0.5, petmoney = 0, useTask = { 2214, 29 }, lvl = 5, },
             [6] = { num = 60000, id = { 6, 1, 1841, 1 }, monster = 100, spe = 1, buffid = 2154, petid = 160, petexp = 0.8, petmoney = 0, useTask = { 2214, 30 }, lvl = 6, },
             [7] = { num = 70000, id = { 6, 1, 1842, 1 }, monster = 110, spe = 0, buffid = 2155, petid = 161, petexp = 0.8, petmoney = 0, useTask = { 2214, 31 }, lvl = 7, },
             [8] = { num = 80000, id = { 6, 1, 1843, 1 }, monster = 110, spe = 0, buffid = 2156, petid = 161, petexp = 0.8, petmoney = 0, useTask = { 2215, 1 }, lvl = 8, },
             [9] = { num = 90000, id = { 6, 1, 1844, 1 }, monster = 110, spe = 1, buffid = 2157, petid = 161, petexp = 1, petmoney = 0, useTask = { 2215, 2 }, lvl = 9, },
             [10] = { num = 90000, id = { 6, 1, 1845, 1 }, monster = 110, spe = 0, buffid = 2158, petid = 162, petexp = 1, petmoney = 0, useTask = { 2215, 3 }, lvl = 10, },
             [11] = { num = 90000, id = { 6, 1, 1861, 1 }, monster = 110, spe = 0, buffid = 2179, petid = 169, petexp = 1, petmoney = 0, useTask = { 2191, 22 }, lvl = 11, itemname = "Kim ThÇn Phi Th¨ng D­¬ng TiÔn" },
           },
  },
  [15] = { petname = "Phi Th¨ng-Kh­¬ng Tö Nha", taskvalue = { 2279, 2280, 2281 }, PetType = 23, taskNoteIdx = { 2080, 2081, 2082, 2122 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Kh­¬ng Tö Nha<c> cã nh÷ng kü n¨ng sau:\n<c=y>[NguyÖn Gi¶ Th­îng C©u]<c> kinh nghiÖm nhiÖm vô LÝnh §¸nh Thuª t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[Th¸i C«ng Tø Phóc]<c> trong chiÕn ®Êu cã x¸c suÊt nhËn ®­îc tr¹ng th¸i Ban phóc, gi¶m s¸t th­¬ng b¹o kÝch g¸nh chÞu ®i 10%.", onlineday = 202012,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1848, 1 }, monster = 90, spe = 0, buffid = 2159, petid = 163, petexp = 0.3, petmoney = 0, useTask = { 2282, 1 }, lvl = 1, },
             [2] = { num = 20000, id = { 6, 1, 1849, 1 }, monster = 90, spe = 0, buffid = 2160, petid = 163, petexp = 0.4, petmoney = 0, useTask = { 2282, 2 }, lvl = 2, },
             [3] = { num = 30000, id = { 6, 1, 1850, 1 }, monster = 90, spe = 1, buffid = 2161, petid = 163, petexp = 0.5, petmoney = 0, useTask = { 2282, 3 }, lvl = 3, },
             [4] = { num = 40000, id = { 6, 1, 1851, 1 }, monster = 100, spe = 0, buffid = 2162, petid = 164, petexp = 0.5, petmoney = 0, useTask = { 2282, 4 }, lvl = 4, },
             [5] = { num = 50000, id = { 6, 1, 1852, 1 }, monster = 100, spe = 0, buffid = 2163, petid = 164, petexp = 0.5, petmoney = 0, useTask = { 2282, 5 }, lvl = 5, },
             [6] = { num = 60000, id = { 6, 1, 1853, 1 }, monster = 100, spe = 1, buffid = 2164, petid = 164, petexp = 0.8, petmoney = 0, useTask = { 2282, 6 }, lvl = 6, },
             [7] = { num = 70000, id = { 6, 1, 1854, 1 }, monster = 110, spe = 0, buffid = 2165, petid = 165, petexp = 0.8, petmoney = 0, useTask = { 2282, 7 }, lvl = 7, },
             [8] = { num = 80000, id = { 6, 1, 1855, 1 }, monster = 110, spe = 0, buffid = 2166, petid = 165, petexp = 0.8, petmoney = 0, useTask = { 2282, 8 }, lvl = 8, },
             [9] = { num = 90000, id = { 6, 1, 1856, 1 }, monster = 110, spe = 1, buffid = 2167, petid = 165, petexp = 1, petmoney = 0, useTask = { 2282, 9 }, lvl = 9, },
             [10] = { num = 90000, id = { 6, 1, 1857, 1 }, monster = 110, spe = 0, buffid = 2168, petid = 166, petexp = 1, petmoney = 0, useTask = { 2282, 10 }, lvl = 10, },
             [11] = { num = 90000, id = { 6, 1, 1862, 1 }, monster = 110, spe = 0, buffid = 2180, petid = 170, petexp = 1, petmoney = 0, useTask = { 2191, 23 }, lvl = 11, itemname = "Kim Th©n Phi Th¨ng Kh­¬ng Tö Nha" },
           },
  },
  [16] = { petname = "Phi Th¨ng-Lý TÞnh", taskvalue = { 2289, 2290, 2291 }, PetType = 24, taskNoteIdx = { 2084, 2085, 2086, 2123 }, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Lý TÞnh<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Thiªn V­¬ng Chi Né]<c> kinh nghiÖm nhiÖm vô HÊp Hån ¢m S¸t t¨ng 100%\n<c=y>[Phó Quý Chi Th©n]<c> cã thÓ sö dông ®Æc quyÒn HuyÒn Vò miÔn phÝ\n<c=y>[B¶o Th¸p Phô Trî]<c> nhËn tr¹ng th¸i hót sinh lùc 2%, hót néi lùc 2%.", onlineday = 202012,
           task = {
             [1] = { num = 10000, id = { 6, 1, 1874, 1 }, monster = 90, spe = 0, buffid = 2185, petid = 171, petexp = 0.3, petmoney = 0, useTask = { 2282, 11 }, lvl = 1, },
             [2] = { num = 20000, id = { 6, 1, 1875, 1 }, monster = 90, spe = 0, buffid = 2186, petid = 171, petexp = 0.4, petmoney = 0, useTask = { 2282, 12 }, lvl = 2, },
             [3] = { num = 30000, id = { 6, 1, 1876, 1 }, monster = 90, spe = 1, buffid = 2187, petid = 171, petexp = 0.5, petmoney = 0, useTask = { 2282, 13 }, lvl = 3, },
             [4] = { num = 40000, id = { 6, 1, 1877, 1 }, monster = 100, spe = 0, buffid = 2188, petid = 172, petexp = 0.5, petmoney = 0, useTask = { 2282, 14 }, lvl = 4, },
             [5] = { num = 50000, id = { 6, 1, 1878, 1 }, monster = 100, spe = 0, buffid = 2189, petid = 172, petexp = 0.5, petmoney = 0, useTask = { 2282, 15 }, lvl = 5, },
             [6] = { num = 60000, id = { 6, 1, 1879, 1 }, monster = 100, spe = 1, buffid = 2190, petid = 172, petexp = 0.8, petmoney = 0, useTask = { 2282, 16 }, lvl = 6, },
             [7] = { num = 70000, id = { 6, 1, 1880, 1 }, monster = 110, spe = 0, buffid = 2191, petid = 173, petexp = 0.8, petmoney = 0, useTask = { 2282, 17 }, lvl = 7, },
             [8] = { num = 80000, id = { 6, 1, 1881, 1 }, monster = 110, spe = 0, buffid = 2192, petid = 173, petexp = 0.8, petmoney = 0, useTask = { 2282, 18 }, lvl = 8, },
             [9] = { num = 90000, id = { 6, 1, 1882, 1 }, monster = 110, spe = 1, buffid = 2193, petid = 173, petexp = 1, petmoney = 0, useTask = { 2282, 19 }, lvl = 9, },
             [10] = { num = 90000, id = { 6, 1, 1883, 1 }, monster = 110, spe = 0, buffid = 2194, petid = 174, petexp = 1, petmoney = 0, useTask = { 2282, 20 }, lvl = 10, },
           },
  },
}

PTAP_TaskTable_AllPet2New = {
  [1] = { petname = "Lôc ¸p §¹o Nh©n", taskvalue = { 2293, 2294 }, PetType = 25, taskNoteIdx = 2124, wenzi = "Khi ®¹t cÊp tèi ®a <c=g>Lôc ¸p §¹o Nh©n<c> cã nh÷ng kü n¨ng sau:\n<c=y>[Tam Tóc Kim ¤]<c> Sinh lùc tèi ®a t¨ng 500 ®iÓm\n<c=y>[Ly Ho¶ Chi Tinh]<c> NÐ tr¸nh t¨ng 100 ®iÓm\n<c=y>[Tr¶m Tiªn Phi §ao]<c>S¸t th­¬ng c¬ b¶n +50, Ho¶ S¸t +30 ®iÓm.", onlineday = 202110, petid = 175, id = { 6, 1, 1887, 1 }, useTask = { 2282, 21 },
          task = {
            [1] = { num = 10000, id = { 6, 1, 1887, 1 }, monster = 90, spe = 0, buffid = 2195, useTask = { 2282, 21 }, lvl = 1, },
            [2] = { num = 20000, id = { 6, 1, 1887, 1 }, monster = 90, spe = 0, buffid = 2195, useTask = { 2282, 21 }, lvl = 2, },
            [3] = { num = 30000, id = { 6, 1, 1887, 1 }, monster = 90, spe = 0, buffid = 2195, useTask = { 2282, 21 }, lvl = 3, },
            [4] = { num = 40000, id = { 6, 1, 1887, 1 }, monster = 90, spe = 0, buffid = 2195, useTask = { 2282, 21 }, lvl = 4, },
            [5] = { num = 50000, id = { 6, 1, 1887, 1 }, monster = 90, spe = 0, buffid = 2195, useTask = { 2282, 21 }, lvl = 5, },
            [6] = { num = 60000, id = { 6, 1, 1887, 1 }, monster = 5, spe = 1, buffid = 2196, useTask = { 2282, 21 }, lvl = 6, },
            [7] = { num = 70000, id = { 6, 1, 1887, 1 }, monster = 5, spe = 1, buffid = 2196, useTask = { 2282, 21 }, lvl = 7, },
            [8] = { num = 80000, id = { 6, 1, 1887, 1 }, monster = 5, spe = 1, buffid = 2196, useTask = { 2282, 21 }, lvl = 8, },
            [9] = { num = 90000, id = { 6, 1, 1887, 1 }, monster = 5, spe = 1, buffid = 2196, useTask = { 2282, 21 }, lvl = 9, },
            [10] = { num = 100000, id = { 6, 1, 1887, 1 }, monster = 5, spe = 1, buffid = 2197, useTask = { 2282, 21 }, lvl = 10, },
          },
  }, }

PTAP_AllPetTable = {
  [1] = { itemid = { 6, 1, 1339, 1 }, buffid = 1784, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 1 }, petid = 46, lvl = 1 },
  [2] = { itemid = { 6, 1, 1340, 1 }, buffid = 1785, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 2 }, petid = 46, lvl = 2 },
  [3] = { itemid = { 6, 1, 1341, 1 }, buffid = 1786, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 3 }, petid = 46, lvl = 3 },
  [4] = { itemid = { 6, 1, 1342, 1 }, buffid = 1787, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 4 }, petid = 47, lvl = 4 },
  [5] = { itemid = { 6, 1, 1343, 1 }, buffid = 1788, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 5 }, petid = 47, lvl = 5 },
  [6] = { itemid = { 6, 1, 1344, 1 }, buffid = 1789, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 6 }, petid = 47, lvl = 6 },
  [7] = { itemid = { 6, 1, 1345, 1 }, buffid = 1790, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 7 }, petid = 48, lvl = 7 },
  [8] = { itemid = { 6, 1, 1346, 1 }, buffid = 1791, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 8 }, petid = 48, lvl = 8 },
  [9] = { itemid = { 6, 1, 1347, 1 }, buffid = 1792, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 9 }, petid = 48, lvl = 9 },
  [10] = { itemid = { 6, 1, 1348, 1 }, buffid = 1793, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Hå Hû MÞ", useTask = { 2188, 10 }, petid = 49, lvl = 10 },


  [11] = { itemid = { 6, 1, 1359, 1 }, buffid = 1808, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 11 }, petid = 50, lvl = 1 },
  [12] = { itemid = { 6, 1, 1360, 1 }, buffid = 1809, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 12 }, petid = 50, lvl = 2 },
  [13] = { itemid = { 6, 1, 1361, 1 }, buffid = 1810, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 13 }, petid = 50, lvl = 3 },
  [14] = { itemid = { 6, 1, 1362, 1 }, buffid = 1811, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 14 }, petid = 51, lvl = 4 },
  [15] = { itemid = { 6, 1, 1363, 1 }, buffid = 1812, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 15 }, petid = 51, lvl = 5 },
  [16] = { itemid = { 6, 1, 1364, 1 }, buffid = 1813, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 16 }, petid = 51, lvl = 6 },
  [17] = { itemid = { 6, 1, 1365, 1 }, buffid = 1814, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 17 }, petid = 52, lvl = 7 },
  [18] = { itemid = { 6, 1, 1366, 1 }, buffid = 1815, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 18 }, petid = 52, lvl = 8 },
  [19] = { itemid = { 6, 1, 1367, 1 }, buffid = 1816, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 19 }, petid = 52, lvl = 9 },
  [20] = { itemid = { 6, 1, 1368, 1 }, buffid = 1817, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Na Tra", useTask = { 2188, 20 }, petid = 53, lvl = 10 },


  [21] = { itemid = { 6, 1, 1426, 1 }, buffid = 1833, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 21 }, petid = 58, lvl = 1 },
  [22] = { itemid = { 6, 1, 1427, 1 }, buffid = 1834, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 22 }, petid = 58, lvl = 2 },
  [23] = { itemid = { 6, 1, 1428, 1 }, buffid = 1835, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 23 }, petid = 58, lvl = 3 },
  [24] = { itemid = { 6, 1, 1429, 1 }, buffid = 1836, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 24 }, petid = 59, lvl = 4 },
  [25] = { itemid = { 6, 1, 1430, 1 }, buffid = 1837, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 25 }, petid = 59, lvl = 5 },
  [26] = { itemid = { 6, 1, 1431, 1 }, buffid = 1838, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 26 }, petid = 59, lvl = 6 },
  [27] = { itemid = { 6, 1, 1432, 1 }, buffid = 1839, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 27 }, petid = 60, lvl = 7 },
  [28] = { itemid = { 6, 1, 1433, 1 }, buffid = 1840, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 28 }, petid = 60, lvl = 8 },
  [29] = { itemid = { 6, 1, 1434, 1 }, buffid = 1841, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 29 }, petid = 60, lvl = 9 },
  [30] = { itemid = { 6, 1, 1435, 1 }, buffid = 1842, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "L«i ChÊn Tö", useTask = { 2188, 30 }, petid = 61, lvl = 10 },


  [31] = { itemid = { 6, 1, 1448, 1 }, buffid = 1847, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2188, 31 }, petid = 62, lvl = 1 },
  [32] = { itemid = { 6, 1, 1449, 1 }, buffid = 1848, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 1 }, petid = 62, lvl = 2 },
  [33] = { itemid = { 6, 1, 1450, 1 }, buffid = 1849, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 2 }, petid = 62, lvl = 3 },
  [34] = { itemid = { 6, 1, 1451, 1 }, buffid = 1850, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 3 }, petid = 63, lvl = 4 },
  [35] = { itemid = { 6, 1, 1452, 1 }, buffid = 1851, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 4 }, petid = 63, lvl = 5 },
  [36] = { itemid = { 6, 1, 1453, 1 }, buffid = 1852, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 5 }, petid = 63, lvl = 6 },
  [37] = { itemid = { 6, 1, 1454, 1 }, buffid = 1853, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 6 }, petid = 64, lvl = 7 },
  [38] = { itemid = { 6, 1, 1455, 1 }, buffid = 1854, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 7 }, petid = 64, lvl = 8 },
  [39] = { itemid = { 6, 1, 1456, 1 }, buffid = 1855, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 8 }, petid = 64, lvl = 9 },
  [40] = { itemid = { 6, 1, 1457, 1 }, buffid = 1856, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Th¹ch C¬ N­¬ng N­¬ng", useTask = { 2189, 9 }, petid = 65, lvl = 10 },


  [41] = { itemid = { 6, 1, 1465, 1 }, buffid = 1861, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 10 }, petid = 66, lvl = 1 },
  [42] = { itemid = { 6, 1, 1466, 1 }, buffid = 1862, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 11 }, petid = 66, lvl = 2 },
  [43] = { itemid = { 6, 1, 1467, 1 }, buffid = 1863, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 12 }, petid = 66, lvl = 3 },
  [44] = { itemid = { 6, 1, 1468, 1 }, buffid = 1864, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 13 }, petid = 67, lvl = 4 },
  [45] = { itemid = { 6, 1, 1469, 1 }, buffid = 1865, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 14 }, petid = 67, lvl = 5 },
  [46] = { itemid = { 6, 1, 1470, 1 }, buffid = 1866, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 15 }, petid = 67, lvl = 6 },
  [47] = { itemid = { 6, 1, 1471, 1 }, buffid = 1867, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 16 }, petid = 68, lvl = 7 },
  [48] = { itemid = { 6, 1, 1472, 1 }, buffid = 1868, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 17 }, petid = 68, lvl = 8 },
  [49] = { itemid = { 6, 1, 1473, 1 }, buffid = 1869, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 18 }, petid = 68, lvl = 9 },
  [50] = { itemid = { 6, 1, 1474, 1 }, buffid = 1870, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Th¸i Êt Ch©n Nh©n", useTask = { 2189, 19 }, petid = 69, lvl = 10 },


  [51] = { itemid = { 6, 1, 1478, 1 }, buffid = 1879, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 20 }, petid = 71, lvl = 1 },
  [52] = { itemid = { 6, 1, 1479, 1 }, buffid = 1880, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 21 }, petid = 71, lvl = 2 },
  [53] = { itemid = { 6, 1, 1480, 1 }, buffid = 1881, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 22 }, petid = 71, lvl = 3 },
  [54] = { itemid = { 6, 1, 1481, 1 }, buffid = 1882, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 23 }, petid = 72, lvl = 4 },
  [55] = { itemid = { 6, 1, 1482, 1 }, buffid = 1883, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 24 }, petid = 72, lvl = 5 },
  [56] = { itemid = { 6, 1, 1483, 1 }, buffid = 1884, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 25 }, petid = 72, lvl = 6 },
  [57] = { itemid = { 6, 1, 1484, 1 }, buffid = 1885, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 26 }, petid = 73, lvl = 7 },
  [58] = { itemid = { 6, 1, 1485, 1 }, buffid = 1886, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 27 }, petid = 73, lvl = 8 },
  [59] = { itemid = { 6, 1, 1486, 1 }, buffid = 1887, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 28 }, petid = 73, lvl = 9 },
  [60] = { itemid = { 6, 1, 1487, 1 }, buffid = 1888, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "§¾c Kû", useTask = { 2189, 29 }, petid = 74, lvl = 10 },


  [61] = { itemid = { 6, 1, 1488, 1 }, buffid = 1889, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2189, 30 }, petid = 75, lvl = 1 },
  [62] = { itemid = { 6, 1, 1489, 1 }, buffid = 1890, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2189, 31 }, petid = 75, lvl = 2 },
  [63] = { itemid = { 6, 1, 1490, 1 }, buffid = 1891, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 1 }, petid = 75, lvl = 3 },
  [64] = { itemid = { 6, 1, 1491, 1 }, buffid = 1892, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 2 }, petid = 76, lvl = 4 },
  [65] = { itemid = { 6, 1, 1492, 1 }, buffid = 1893, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 3 }, petid = 76, lvl = 5 },
  [66] = { itemid = { 6, 1, 1493, 1 }, buffid = 1894, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 4 }, petid = 76, lvl = 6 },
  [67] = { itemid = { 6, 1, 1494, 1 }, buffid = 1895, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 5 }, petid = 77, lvl = 7 },
  [68] = { itemid = { 6, 1, 1495, 1 }, buffid = 1896, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 6 }, petid = 77, lvl = 8 },
  [69] = { itemid = { 6, 1, 1496, 1 }, buffid = 1897, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 7 }, petid = 77, lvl = 9 },
  [70] = { itemid = { 6, 1, 1497, 1 }, buffid = 1898, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Th©n C«ng B¸o", useTask = { 2190, 8 }, petid = 78, lvl = 10 },


  [71] = { itemid = { 6, 1, 1498, 1 }, buffid = 1899, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 9 }, petid = 79, lvl = 1 },
  [72] = { itemid = { 6, 1, 1499, 1 }, buffid = 1900, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 10 }, petid = 79, lvl = 2 },
  [73] = { itemid = { 6, 1, 1500, 1 }, buffid = 1901, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 11 }, petid = 79, lvl = 3 },
  [74] = { itemid = { 6, 1, 1501, 1 }, buffid = 1902, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 12 }, petid = 80, lvl = 4 },
  [75] = { itemid = { 6, 1, 1502, 1 }, buffid = 1903, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 13 }, petid = 80, lvl = 5 },
  [76] = { itemid = { 6, 1, 1503, 1 }, buffid = 1904, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 14 }, petid = 80, lvl = 6 },
  [77] = { itemid = { 6, 1, 1504, 1 }, buffid = 1905, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 15 }, petid = 81, lvl = 7 },
  [78] = { itemid = { 6, 1, 1505, 1 }, buffid = 1906, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 16 }, petid = 81, lvl = 8 },
  [79] = { itemid = { 6, 1, 1506, 1 }, buffid = 1907, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 17 }, petid = 81, lvl = 9 },
  [80] = { itemid = { 6, 1, 1507, 1 }, buffid = 1908, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Hoµng Phi Hæ", useTask = { 2190, 18 }, petid = 82, lvl = 10 },

  [81] = { itemid = { 6, 1, 1541, 1 }, buffid = 1936, taskvalue = { 2071, 2072, 2073 }, PetType = 1, itemname = "Kim Kª Hå HØ MÞ", useTask = { 2191, 1 }, petid = 83, lvl = 11 },
  [82] = { itemid = { 6, 1, 1542, 1 }, buffid = 1937, taskvalue = { 2078, 2079, 2080 }, PetType = 2, itemname = "Kim Th©n Na Tra", useTask = { 2191, 2 }, petid = 84, lvl = 11 },
  [83] = { itemid = { 6, 1, 1543, 1 }, buffid = 1938, taskvalue = { 2091, 2092, 2093 }, PetType = 3, itemname = "Kim Vò L«i ChÊn Tö", useTask = { 2191, 3 }, petid = 85, lvl = 11 },
  [84] = { itemid = { 6, 1, 1544, 1 }, buffid = 1939, taskvalue = { 2106, 2107, 2108 }, PetType = 4, itemname = "Kim Quan Th¹ch C¬", useTask = { 2191, 4 }, petid = 86, lvl = 11 },
  [85] = { itemid = { 6, 1, 1475, 1 }, buffid = 1940, taskvalue = { 2114, 2115, 2116 }, PetType = 5, itemname = "Kim Tiªn Th¸i Êt", useTask = { 2191, 5 }, petid = 70, lvl = 11 },
  [86] = { itemid = { 6, 1, 1660, 1 }, buffid = 2007, taskvalue = { 2117, 2118, 2119 }, PetType = 6, itemname = "Kim Hå §¸t Kû", useTask = { 2191, 6 }, petid = 109, lvl = 11 },
  [87] = { itemid = { 6, 1, 1661, 1 }, buffid = 2008, taskvalue = { 2120, 2121, 2122 }, PetType = 7, itemname = "Kim B¸o Th©n C«ng B¸o", useTask = { 2191, 7 }, petid = 110, lvl = 11 },
  [88] = { itemid = { 6, 1, 1662, 1 }, buffid = 2009, taskvalue = { 2123, 2124, 2125 }, PetType = 8, itemname = "Kim §¶m Hoµng Phi Hæ", useTask = { 2191, 8 }, petid = 111, lvl = 11 },


}

